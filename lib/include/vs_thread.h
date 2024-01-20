#ifndef _VS_THREAD_H
#define _VS_THREAD_H

#include <thread>
#include "revision.h"

namespace vs {

    /**
     * @brief std::thread wrapper with additional logic for managing Revisions
     */
    class thread : private std::thread {
    public:

        /**
         * @brief Construcs thread and creates new Revision for it
         *
         * @tparam Function - callable
         * @tparam Args - optional arguments for function
         * @param f - function to call
         * @param args - args to pass to the function (optional)
         */
        template <typename Function, typename... Args>
        explicit thread(Function&& f, Args&&... args) : std::thread(&thread::threadFunctionWrapper<Function, Args...>, this,
                          std::forward<Function>(f), std::forward<Args>(args)...) {
            auto s = std::make_shared<Segment>(Revision::currentRevision->current);
            threadRevision = std::make_shared<Revision>(Revision::currentRevision->current, s);
            Revision::currentRevision->current->Release();
            Revision::currentRevision->current = std::make_shared<Segment>(Revision::currentRevision->current);
        }

        /**
         * @brief Joins thread and it's Revision
         *
         * @see Revision
         */
        void join() {
            std::thread::join();

            std::shared_ptr<Segment> s = threadRevision->current;
            while (s != threadRevision->root) {
                for (auto v: s->written) {
                    v->Merge(Revision::currentRevision, threadRevision, s);
                }
                s = s->parent;
            }

            threadRevision->current->Release();
            Revision::currentRevision->current->Collapse(Revision::currentRevision);
        }

        // Don't allow to detach thread
        // using std::thread::detach;

        using std::thread::get_id;

    private:
        /**
         * @brief Revision of this thread
         *
         * Needed for joining and threadFunctionWrapper
         */
        std::shared_ptr<Revision> threadRevision;

        /**
         * @brief Function that will be called in thread before passed thread function
         *
         * @param self Needed for accessing threadRevision
         */
        template <class Function, class... Args>
        static void threadFunctionWrapper(thread* self, Function&& f, Args&&... args) {
            Revision::currentRevision = self->threadRevision;

            std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
        }


    };

}

#endif

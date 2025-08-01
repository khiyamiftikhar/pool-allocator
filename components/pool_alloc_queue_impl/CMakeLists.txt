idf_component_register(SRCS "pool_queue.c"
                        INCLUDE_DIRS "."
                        PRIV_REQUIRES pool_alloc_interface
                        )
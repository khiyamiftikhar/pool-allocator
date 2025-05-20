idf_component_register(SRCS "pool-allocator.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES pool_alloc_queue_impl segger-sysview)

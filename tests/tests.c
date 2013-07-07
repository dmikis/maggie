#include <stdlib.h>

#include <check.h>

#include <parser.h>

typedef Suite * (* create_suite_proc_t)(void);

int main(void) {
    static create_suite_proc_t procs[] = {
        parser_suite_create,
        NULL
    };
    size_t i = 0;
    create_suite_proc_t proc;

    while ((proc = procs[i++])) {
        SRunner * runner = srunner_create((*proc)());
        srunner_run_all(runner, CK_MINIMAL);
        if (srunner_ntests_failed(runner)) {
            return EXIT_FAILURE;
        }
        srunner_free(runner);
        proc = proc + 1;
    }

    return EXIT_SUCCESS;
}

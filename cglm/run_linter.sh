#!/bin/sh

SPLINT_ARGS="-I . -fcnuse -usedef -compdef"

echo "\n\n\n>>> RUNNING LINTER ON cglm_tests.c BEGIN"
splint cglm_tests.c ${SPLINT_ARGS}
echo ">>> RUNNING LINTER ON cglm_tests.c END"

echo "\n\n\n>>> RUNNING LINTER ON cglm_bench.c BEGIN"
splint cglm_bench.c ${SPLINT_ARGS}
echo ">>> RUNNING LINTER ON cglm_bench.c END"

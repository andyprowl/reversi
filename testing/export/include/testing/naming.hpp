#pragma once

#define PASTE_1(x, y) x ## _ ## y
#define PASTE_2(x, y) PASTE_1(x, y)

#define TEST_THAT_RESOLVER(F, WORK_UNIT_SCENARIO, BEHAVIOR) \
        TEST_F(F, PASTE_2(WORK_UNIT_SCENARIO, BEHAVIOR))

#define TEST_THAT(F, WORK_UNIT, SCENARIO, BEHAVIOR) \
        TEST_THAT_RESOLVER(F, PASTE_2(WORK_UNIT, SCENARIO), BEHAVIOR)

#define WHAT(X) X

#define WHEN(X) X

#define ALWAYS() WHEN(Always)

#define THEN(X) X
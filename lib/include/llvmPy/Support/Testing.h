#pragma once

// __mock_virtual methods can be stubbed and spied in tests.

#ifndef NDEBUG
#define MOCK_VIRTUAL virtual
#else
#define MOCK_VIRTUAL
#endif

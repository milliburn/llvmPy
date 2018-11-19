#pragma once

// __mock_virtual methods can be stubbed and spied in tests.

#ifndef NDEBUG
#define __mock_virtual virtual
#else
#define __mock_virtual
#endif

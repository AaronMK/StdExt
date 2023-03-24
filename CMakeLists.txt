cmake_minimum_required(VERSION 3.12.0)
project(StdExt)

list(APPEND CMAKE_PREFIX_PATH include)

set(PUBLIC_HEADERS
    include/StdExt/Any.h
    include/StdExt/Buffer.h
    include/StdExt/CallableTraits.h
    include/StdExt/Compare.h
    include/StdExt/Concepts.h
    include/StdExt/Exceptions.h
    include/StdExt/FunctionPtr.h
    include/StdExt/InPlace.h
    include/StdExt/Matrix.h
    include/StdExt/Number.h
    include/StdExt/Platform.h
    include/StdExt/StdExt.h
    include/StdExt/String.h
    include/StdExt/Type.h
    include/StdExt/TypeInfo.h
    include/StdExt/UID.h
    include/StdExt/Utility.h
    include/StdExt/Vec.h
    include/StdExt/Collections/Collections.h
    include/StdExt/Collections/SharedArray.h
    include/StdExt/Collections/Vector.h
    include/StdExt/Concurrent/Condition.h
    include/StdExt/Concurrent/CallableTask.h
    include/StdExt/Concurrent/FunctionTask.h
    include/StdExt/Concurrent/MessageLoop.h
    include/StdExt/Concurrent/Mutex.h
    include/StdExt/Concurrent/PredicatedCondition.h
    include/StdExt/Concurrent/Producer.h
    include/StdExt/Concurrent/Queue.h
    include/StdExt/Concurrent/RWLock.h
    include/StdExt/Concurrent/Task.h
    include/StdExt/Concurrent/TaskLoop.h
    include/StdExt/Concurrent/Timer.h
    include/StdExt/Concurrent/Wait.h
    include/StdExt/Memory/Alignment.h
    include/StdExt/Memory/Casting.h
    include/StdExt/Memory/Endianess.h
    include/StdExt/Memory/TaggedPtr.h
    include/StdExt/Memory/SharedData.h
    include/StdExt/Serialize/Exceptions.h
    include/StdExt/Serialize/Serialize.h
    include/StdExt/Serialize/Binary/Binary.h
    include/StdExt/Serialize/Text/Text.h
    include/StdExt/Serialize/XML/Element.h
    include/StdExt/Serialize/XML/XML.h
    include/StdExt/Signals/CallableHandler.h
    include/StdExt/Signals/Constant.h
    include/StdExt/Signals/Event.h
    include/StdExt/Signals/EventHandler.h
    include/StdExt/Signals/FunctionHandlers.h
    include/StdExt/Signals/Invokable.h
    include/StdExt/Signals/Settable.h
    include/StdExt/Signals/Subscription.h
    include/StdExt/Signals/Watchable.h
    include/StdExt/Streams/BufferedStream.h
    include/StdExt/Streams/ByteStream.h
    include/StdExt/Streams/FileStream.h
    include/StdExt/Streams/MemoryStream.h
    include/StdExt/Streams/TestByteStream.h
    include/StdExt/Test/Test.h
    include/StdExt/Unicode/Iterator.h
)

set(PRIVATE_HEADERS
    src/Serialize/TinyXml2/tinyxml2.h
    src/Serialize/XML/ElementInternal.h
)

set(SOURCES
    src/Buffer.cpp
    src/Matrix.cpp
    src/Number.cpp
    src/String.cpp
    src/UID.cpp
    src/Vec.cpp
    src/Concurrent/Condition.cpp
    src/Concurrent/FunctionTask.cpp
    src/Concurrent/Mutex.cpp
    src/Concurrent/RWLock.cpp
    src/Concurrent/Task.cpp
    src/Concurrent/TaskLoop.cpp
    src/Concurrent/Timer.cpp
    src/Concurrent/Wait.cpp
    src/Serialize/Exceptions.cpp
    src/Serialize/Binary/Binary.cpp
    src/Serialize/Text/Text.cpp
    src/Serialize/TinyXml2/tinyxml2.cpp
    src/Serialize/XML/Element.cpp
    src/Serialize/XML/ElementInternal.cpp
    src/Serialize/XML/XML.cpp
    src/Streams/BufferedStream.cpp
    src/Streams/ByteStream.cpp
    src/Streams/FileStream.cpp
    src/Streams/MemoryStream.cpp
    src/Streams/TestByteStream.cpp
)

add_library(StdExt
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
    ${SOURCES}
)

target_include_directories(StdExt
    PUBLIC include
)

target_compile_features(StdExt PUBLIC cxx_std_20)

set(TEST_SOURCES
    test/Any_Test.cpp
    test/CallableTraits_test.cpp
    test/Collections_Test.cpp
    test/Concepts_Test.cpp
    test/Concurrent_Test.cpp
    test/InPlace_test.cpp
    test/main.cpp
    test/Matrix_Test.cpp
    test/Memory_test.cpp
    test/Number_Test.cpp
    test/Serialize_test.cpp
    test/SerializeTesting.h
    test/Signal_Test.cpp
    test/String_Test.cpp
    test/TestClasses.cpp
    test/TestClasses.h
    test/TypeInfo_test.cpp
    test/Unicode_Test.cpp
    test/Utility_test.cpp
    test/Vec_Test.cpp
)

add_executable(test
    ${TEST_SOURCES}
)

target_link_libraries(test StdExt)

target_include_directories(test
    PUBLIC include
)

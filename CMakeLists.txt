cmake_minimum_required(VERSION 3.12.0)
project(StdExt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(
	Doxygen
	OPTIONAL_COMPONENTS dot
)

list(APPEND CMAKE_PREFIX_PATH include)

set(STD_EXT_PUBLIC_HEADERS
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
	include/StdExt/TemplateUtility.h
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
	include/StdExt/Concurrent/Mutex.h
	include/StdExt/Concurrent/PredicatedCondition.h
	include/StdExt/Concurrent/Producer.h
	include/StdExt/Concurrent/Queue.h
	include/StdExt/Concurrent/RWLock.h
	include/StdExt/Concurrent/Task.h
	include/StdExt/Concurrent/Timer.h
	include/StdExt/Concurrent/Wait.h
	include/StdExt/Defaultable.h
	include/StdExt/IpComm/Endpoint.h
	include/StdExt/IpComm/Exceptions.h
	include/StdExt/IpComm/IpAddress.h
	include/StdExt/IpComm/IpComm.h
	include/StdExt/IpComm/NetworkInterface.h
	include/StdExt/IpComm/SockAddr.h
	include/StdExt/IpComm/TcpConnection.h
	include/StdExt/IpComm/TcpServer.h
	include/StdExt/IpComm/Udp.h
	include/StdExt/Memory/Alignment.h
	include/StdExt/Memory/BitMask.h
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
)

set(STD_EXT_PRIVATE_HEADERS
	src/IpComm/IpCommOpaque.h
	src/Serialize/XML/ElementInternal.h
)

set(STD_EXT_SOURCES
	src/Buffer.cpp
	src/Exceptions.cpp
	src/Matrix.cpp
	src/Number.cpp
	src/String.cpp
	src/UID.cpp
	src/Vec.cpp
	src/Concurrent/Condition.cpp
	src/Concurrent/Mutex.cpp
	src/Concurrent/RWLock.cpp
	src/Concurrent/Scheduler.cpp
	src/Concurrent/Task.cpp
	src/Concurrent/Timer.cpp
	src/Concurrent/Wait.cpp
	src/IpComm/Endpoint.cpp
	src/IpComm/IpAddress.cpp
	src/IpComm/IpCommOpaque.cpp
	src/IpComm/IpExceptions.cpp
	src/IpComm/NetworkInterface.cpp
	src/IpComm/SockAddr.cpp
	src/IpComm/TcpConnection.cpp
	src/IpComm/TcpServer.cpp
	src/IpComm/Udp.cpp
	src/Memory/Alignment.cpp
	src/Serialize/SerializeExceptions.cpp
	src/Serialize/Binary/Binary.cpp
	src/Serialize/Text/Text.cpp
	src/Serialize/XML/Element.cpp
	src/Serialize/XML/ElementInternal.cpp
	src/Serialize/XML/XML.cpp
	src/Streams/BufferedStream.cpp
	src/Streams/ByteStream.cpp
	src/Streams/FileStream.cpp
	src/Streams/MemoryStream.cpp
	src/Streams/SocketStream.cpp
	src/Streams/TestByteStream.cpp
)

set(STD_EXT_EXTERNAL_SOURCES
	src/Serialize/TinyXml2/tinyxml2.h
	src/Serialize/TinyXml2/tinyxml2.cpp
)

add_library(StdExt
	${STD_EXT_PUBLIC_HEADERS}
	${STD_EXT_PRIVATE_HEADERS}
	${STD_EXT_SOURCES}
	${STD_EXT_EXTERNAL_SOURCES}
)

target_include_directories(StdExt
	PUBLIC include
)

IF (APPLE)
	find_package(Iconv REQUIRED)
	target_include_directories(StdExt
		PRIVATE ${Iconv_INCLUDE_DIRS}
	)
	target_link_libraries(StdExt
		INTERFACE ${Iconv_LIBRARIES}
	)
endif()

set(TEST_SOURCES
	test/Any_Test.cpp
	test/CallableTraits_test.cpp
	test/Callable_test.cpp
	test/Collections_Test.cpp
	test/Concepts_Test.cpp
	test/Concurrent_Test.cpp
	test/Const_test.cpp
	test/Defaultable_test.cpp
	test/InPlace_test.cpp
	test/IpComm_test.cpp
	test/main.cpp
	test/Matrix_Test.cpp
	test/Memory_test.cpp
	test/Number_Test.cpp
	test/Serialize_test.cpp
	test/SerializeTesting.h
	test/Signal_Test.cpp
	test/Stream_Test.cpp
	test/String_Test.cpp
	test/TemplateUtility_test.cpp
	test/TestClasses.cpp
	test/TestClasses.h
	test/TypeInfo_test.cpp
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

if(DOXYGEN_FOUND)
	set(DOXYGEN_GENERATE_HTML        YES)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	set(DOXYGEN_OUTPUT_DIRECTORY     ${CMAKE_CURRENT_SOURCE_DIR}/Docs)
	set(DOXYGEN_SOURCE_BROWSER       YES)
	set(DOXYGEN_EXTRACT_ALL          YES)
	set(DOXYGEN_EXTRACT_PRIVATE      YES)
	set(DOXYGEN_EXTRACT_STATIC       YES)

	doxygen_add_docs(StdExtDocs
		${STD_EXT_PUBLIC_HEADERS}
		${STD_EXT_PRIVATE_HEADERS}
		${STD_EXT_SOURCES}

		USE_STAMP_FILE
	)
endif()
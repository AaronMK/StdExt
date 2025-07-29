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
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Any.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Buffer.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Callable.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Compare.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Concepts.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Defaultable.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Exceptions.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/FunctionTraits.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/InPlace.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Matrix.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Number.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Platform.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/StdExt.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/String.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/TemplateUtility.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Type.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/TypeInfo.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Utility.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Vec.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Collections/Collections.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Collections/SharedArray.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Collections/Vector.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Concurrent/Timer.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/Alignment.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/BitMask.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/Casting.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/Endianess.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/SharedData.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/TaggedPtr.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Memory/Utility.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/Exceptions.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/Serialize.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/Binary/Binary.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/Text/Text.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/XML/Element.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Serialize/XML/XML.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/CallableHandler.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Constant.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Event.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/EventHandler.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/FunctionHandlers.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Invokable.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Settable.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Subscription.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Signals/Watchable.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/BufferedStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/ByteStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/FileStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/MemoryStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/SocketStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Streams/TestByteStream.h
	${CMAKE_CURRENT_LIST_DIR}/include/StdExt/Test/Test.h
)

set(STD_EXT_PRIVATE_HEADERS
	src/Serialize/XML/ElementInternal.h
)

set(STD_EXT_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/Buffer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Exceptions.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Matrix.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Number.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/String.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Vec.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Concurrent/Timer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Memory/Alignment.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/SerializeExceptions.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/Binary/Binary.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/Text/Text.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/XML/Element.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/XML/ElementInternal.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/XML/XML.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/BufferedStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/ByteStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/FileStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/MemoryStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/SocketStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Streams/TestByteStream.cpp
)

set(STD_EXT_EXTERNAL_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/TinyXml2/tinyxml2.h
	${CMAKE_CURRENT_LIST_DIR}/src/Serialize/TinyXml2/tinyxml2.cpp
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
	${CMAKE_CURRENT_LIST_DIR}/test/Concurrent/Timer_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Any_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Callable_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Collections_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Concepts_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Const_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Defaultable_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/FunctionTraits_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/InPlace_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Matrix_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Memory_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Number_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Serialize_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/SerializeTesting.h
	${CMAKE_CURRENT_LIST_DIR}/test/Signal_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Stream_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/String_Test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/TemplateUtility_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/TestClasses.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/TestClasses.h
	${CMAKE_CURRENT_LIST_DIR}/test/TypeInfo_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Utility_test.cpp
	${CMAKE_CURRENT_LIST_DIR}/test/Vec_Test.cpp
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
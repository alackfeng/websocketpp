
## 编译说明


#### mac - 
`````

mkdir build; cd build
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DOPENSSL_ROOT_DIR="/usr/local/opt/openssl"
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DSDK_WEBSOCKET_TLS=ON -DOPENSSL_ROOT_DIR="/usr/local/opt/openssl" -DOPENSSL_INCLUDE_DIR="/usr/local/opt/openssl/include"

`````

#### windows - vs 2017
`````


## - cmd

#### 依赖 boost openssl zlib


set BOOST_ROOT=C:\local\boost_1_62_0
set OPENSSL_ROOT=C:\local\OpenSSL-Win64
set OPENSSL_CONF=%OPENSSL_ROOT%\bin\openssl.cfg
set ZLIB_LIBRARY=C:\local\zlib-1.2.11

mkdir build;
cd build;
cmake .. -G "Visual Studio 15 2017 Win64" -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DBOOST_ROOT=C:\local\boost_1_62_0 -DOPENSSL_INCLUDE_DIR=C:\local\OpenSSL-Win64\include -DOPENSSL_LIBRARIES=C:\local\OpenSSL-Win64\lib\libcrypto_static.lib;C:\local\OpenSSL-Win64\lib\libssl_static.lib -DZLIB_LIBRARY=C:\local\zlib-1.2.11\libs\zlibstatic.lib -DZLIB_INCLUDE_DIR=C:\local\zlib-1.2.11\include



Q1: C:\local\boost_1_62_0\boost/asio/ssl/detail/openssl_types.hpp(19): fatal error C1083: 无法打开包括文件: “openssl/conf.h”: No such file or directory
A1: cmake/CMakeHelpers.cmake =>

macro (link_boost)
    if(MSVC)
        include_directories (${OPENSSL_INCLUDE_DIR})
    endif()
    target_link_libraries (${TARGET_NAME} ${Boost_LIBRARIES})
endmacro ()


Q2: LNK2001 无法解析的外部符号 __imp_ntohs	LNK2019 无法解析的外部符号 __imp_ntohs
A2: CMakeLists.txt => MSVC -> set (WEBSOCKETPP_PLATFORM_LIBS ws2_32)

Q3: integration.cpp(319): error C3861: “sleep”: 找不到标识符
A3: 
#ifdef WIN32
#define sleep(a) Sleep(a*1000)
#endif // WIN32


Q4: websocketpp/transport/asio/security/tls.hpp(358): error C2065: “SSL_R_SHORT_READ”: 未声明的标识符
A4: 
#ifdef SSL_R_SHORT_READ
            if (ERR_GET_REASON(ec.value()) == SSL_R_SHORT_READ) {
                return make_error_code(transport::error::tls_short_read);
            } else {
#endif
                // We know it is a TLS related error, but otherwise don't know
                // more. Pass through as TLS generic.
                return make_error_code(transport::error::tls_error);
#ifdef SSL_R_SHORT_READ
            }
#endif



`````

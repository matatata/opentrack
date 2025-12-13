# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

include_guard(GLOBAL)


if(APPLE)
    set(macos-application-support-dir "Library/Application Support")
    set(opentrack-bin "Applications")
    set(opentrack-libexec "${opentrack-bin}/opentrack.app/Contents/PlugIns/opentrack")
    set(opentrack-runtime-libexec "/../PlugIns/opentrack/")                        # MUST HAVE A TRAILING BACKSLASH, Used in APP
    set(opentrack-runtime-doc "/../Resources/")
    set(opentrack-aux "${macos-application-support-dir}/opentrack")
    set(opentrack-doc "${opentrack-aux}/doc")
    set(opentrack-game-csv-doc "${opentrack-bin}/opentrack.app/Contents/Resources") # settings/freetracknoir supported games.csv goes here
    set(opentrack-i18n "${opentrack-bin}/opentrack.app/Contents/Resources") # used during install
    set(opentrack-runtime-i18n "../Resources/i18n") # used in application
    set(opentrack-install-rpath "@executable_path/../Frameworks")
    set(opentrack-install-xplane "${opentrack-aux}/X-Plane-Plugin")

    set(opentrack-install-winebridge "${macos-application-support-dir}/opentrack-wine-bridge")
    set(opentrack-install-wine-unixlib "${opentrack-install-winebridge}/lib/wine")
    set(opentrack-install-winebridge-bin "${opentrack-install-winebridge}/bin")
    set(opentrack-install-opentrackclient-prefix "${opentrack-aux}")
    set(opentrack-install-opentrackclient-include "${opentrack-install-opentrackclient-prefix}/include")
    set(opentrack-install-opentrackclient-lib "${opentrack-install-opentrackclient-prefix}/lib")
    set(opentrack-runtime-opentrackclient-lib-rpath "${CMAKE_INSTALL_PREFIX}/${opentrack-install-opentrackclient-lib}")
    set(opentrack-install-opentrackclient-framework "${opentrack-aux}/Frameworks")
    set(opentrack-runtime-opentrackclient-framework-rpath "${CMAKE_INSTALL_PREFIX}/${opentrack-install-opentrackclient-framework}")
    set(opentrack-install-opentrackclient-sdk "${opentrack-aux}/SDK")

elseif(WIN32)
    set(opentrack-libexec "modules")
    set(opentrack-runtime-libexec "/${opentrack-libexec}/")  # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-runtime-doc "/doc/")                     # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-bin ".")
    set(opentrack-doc "doc")
    set(opentrack-game-csv-doc "doc")  # settings/freetracknoir supported games.csv goes here
    set(opentrack-i18n "i18n")
    set(opentrack-runtime-i18n "./i18n")
    set(opentrack-debug "debug")
    set(opentrack-install-rpath "")
else()
    set(opentrack-libexec "libexec/opentrack")
    set(opentrack-runtime-libexec "/../${opentrack-libexec}/")   # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-runtime-doc "/../share/doc/opentrack/")         # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-bin "bin")
    set(opentrack-aux "share/opentrack")
    set(opentrack-doc "share/doc/opentrack")
    set(opentrack-game-csv-doc "share/doc/opentrack")  # settings/freetracknoir supported games.csv goes here
    set(opentrack-install-rpath "${CMAKE_INSTALL_PREFIX}/${opentrack-libexec}")
    set(opentrack-i18n "share/opentrack/i18n")
    set(opentrack-runtime-i18n "../share/opentrack/i18n")
    set(opentrack-install-xplane "${opentrack-aux}/xplane")

    set(opentrack-install-winebridge "share/opentrack-wine-bridge")
    set(opentrack-install-wine-unixlib "${opentrack-install-winebridge}/lib/wine")
    set(opentrack-install-winebridge-bin "${opentrack-install-winebridge}/bin")
    set(opentrack-install-opentrackclient-prefix "${CMAKE_INSTALL_PREFIX}")
    set(opentrack-install-opentrackclient-include "${opentrack-install-opentrackclient-prefix}/include")
    set(opentrack-install-opentrackclient-lib "${opentrack-install-opentrackclient-prefix}/lib")
    set(opentrack-runtime-opentrackclient-lib-rpath "${CMAKE_INSTALL_PREFIX}/${opentrack-install-opentrackclient-lib}")
    set(opentrack-install-opentrackclient-sdk "${opentrack-aux}/SDK")

endif()

function(otr_escape_string var str)
    string(REGEX REPLACE "([\\\"$;])" "\\\\\\1" str "${str}")
    string(REPLACE "\n" "\\n" str "${str}")
    set(${var} "${str}" PARENT_SCOPE)
endfunction()

set(opentrack-contrib-pfx "${opentrack-doc}/contrib")

set(opentrack-binary-suffix "")
if(WIN32)
    set(opentrack-binary-suffix ".exe")
endif()

set(CMAKE_INSTALL_RPATH "${opentrack-install-rpath}")



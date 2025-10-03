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
    set(opentrack-bin "${CMAKE_INSTALL_PREFIX}")
    set(opentrack-libexec "${opentrack-bin}/opentrack.app/Contents/PlugIns/opentrack")
    set(opentrack-runtime-libexec "/../PlugIns/opentrack/")                        # MUST HAVE A TRAILING BACKSLASH, Used in APP
    set(opentrack-runtime-doc "/../Resources/")                         # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-doc "doc")
    set(opentrack-game-csv-doc "${opentrack-bin}/opentrack.app/Contents/Resources") # settings/freetracknoir supported games.csv goes here
    set(opentrack-i18n "${opentrack-bin}/opentrack.app/Contents/Resources") # used during install
    set(opentrack-runtime-i18n "../Resources/i18n") # used in application
    set(opentrack-install-rpath "@executable_path/../Frameworks")
    set(opentrack-install-wine-unixlib "Wine/lib/wine") # Where should the builtin wine libs be installed? To install into an existing wine-installation chosse something like /opt/local/lib/wine or /Applications/Wine\ Stable.app/Contents/Resources/wine/lib/wine
    set(opentrack-install-ftnoir-registry-tool "Wine/FTnoirRegistryTool") # Where should the registry tool be installed?
    set(opentrack-install-freetrackclient-posix "usr/local") # Where should the SDK be installed?
    set(opentrack-runtime-freetrackclient-posix-rpath "/usr/local/lib") # Where will the library be found
elseif(WIN32)
    set(opentrack-libexec "modules")
    set(opentrack-runtime-libexec "/${opentrack-libexec}/")  # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-runtime-doc "/doc/")                     # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-bin ".")
    set(opentrack-doc "doc")
    set(opentrack-game-csv-doc "doc")  # settings/freetracknoir supported games.csv goes here
    set(opentrack-i18n "i18n")
    set(opentrack-runtime-i18n "./i18n")
    set(opentrack-debug "./debug")
    set(opentrack-install-rpath "")
else()
    set(opentrack-libexec "libexec/opentrack")
    set(opentrack-runtime-libexec "/../${opentrack-libexec}/")   # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-runtime-doc "/../share/doc/opentrack/")         # MUST HAVE A TRAILING BACKSLASH
    set(opentrack-bin "bin")
    set(opentrack-doc "share/doc/opentrack")
    set(opentrack-game-csv-doc "share/doc/opentrack")  # settings/freetracknoir supported games.csv goes here
    set(opentrack-install-rpath "${CMAKE_INSTALL_PREFIX}/${opentrack-libexec}")
    set(opentrack-i18n "share/opentrack/i18n")
    set(opentrack-runtime-i18n "../share/opentrack/i18n")
    set(opentrack-install-wine-unixlib "${opentrack-libexec}/wine/lib/wine") # Where should the builtin wine libs be installed? To install into an existing wine-installation chosse something like /opt/local/lib/wine
    set(opentrack-install-ftnoir-registry-tool "${opentrack-libexec}/wine/FTnoirRegistryTool") # Where should the registry tool be installed?
    set(opentrack-install-freetrackclient-posix "usr/local") # Where should the SDK be installed?
    set(opentrack-runtime-freetrackclient-posix-rpath "/usr/local/lib") # Where will the library be found
endif()

function(otr_escape_string var str)
    string(REGEX REPLACE "([^_A-Za-z0-9./:-])" "\\\\\\1" str "${str}")
    set(${var} "${str}" PARENT_SCOPE)
endfunction()

set(opentrack-contrib-pfx "${opentrack-doc}/contrib")

set(opentrack-binary-suffix "")
if(WIN32)
    set(opentrack-binary-suffix ".exe")
endif()

set(CMAKE_INSTALL_RPATH "${opentrack-install-rpath}")



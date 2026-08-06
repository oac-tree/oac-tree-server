# -----------------------------------------------------------------------------
# Common project configuration
# -----------------------------------------------------------------------------

include(COACommonConfig)
include(COASetupCodacEnvironment)
include(FetchGithubDeps)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(sup-utils REQUIRED)
find_package(sup-dto REQUIRED)
find_package(sup-epics REQUIRED)
find_package(oac-tree REQUIRED)
find_package(sup-protocol REQUIRED)
find_package(Asio REQUIRED)
find_package(OpenSSL REQUIRED)

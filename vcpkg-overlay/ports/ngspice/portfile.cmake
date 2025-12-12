vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)

# ngspice produces self-contained DLLs
set(VCPKG_CRT_LINKAGE static)

# Download from SourceForge with fallback URLs
# Note: The SHA512 matches the repacked tarball from GitHub (directory renamed from ngspice-ngspice-41 to ngspice-41)
vcpkg_download_distfile(ARCHIVE
    URLS
        "https://downloads.sourceforge.net/project/ngspice/ng-spice-rework/41/ngspice-41.tar.gz"
        "https://sourceforge.net/projects/ngspice/files/ng-spice-rework/41/ngspice-41.tar.gz/download"
    FILENAME "ngspice-41.tar.gz"
    SHA512 8cdcca13cae64e195ff1c04d2485a600136f71db6d84ed1069d2a6564200050d5a4746dce46fd0dd9d4d91cc1e10f68f01417ee463bbb9e4e2f1eedf5fedc85e
    # If automatic download fails, manually download from GitHub and repack:
    # 1. Download: https://github.com/ngspice/ngspice/archive/refs/tags/ngspice-41.tar.gz
    # 2. Extract, rename directory from ngspice-ngspice-41 to ngspice-41
    # 3. Repack as ngspice-41.tar.gz
    # 4. Place in vcpkg downloads folder
)

vcpkg_extract_source_archive(
    SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
    PATCHES
        fix-winbison-path.patch
        Fix-C2065.patch
        remove-post-build.patch
        remove-64-in-codemodel-name.patch
        fftw3-tweaks.patch
        add-ARM64-arch-to-the-msbuild-projects.patch
)

vcpkg_find_acquire_program(BISON)

get_filename_component(BISON_DIR "${BISON}" DIRECTORY)
vcpkg_add_to_path(PREPEND "${BISON_DIR}")

# Sadly, vcpkg globs .libs inside install_msbuild and whines that the 47 year old SPICE format isn't a MSVC lib ;)
# We need to kill them off first before the source tree is copied to a tmp location by install_msbuild

file(REMOVE_RECURSE "${SOURCE_PATH}/contrib")
file(REMOVE_RECURSE "${SOURCE_PATH}/examples")
file(REMOVE_RECURSE "${SOURCE_PATH}/man")
file(REMOVE_RECURSE "${SOURCE_PATH}/tests")

# this builds the main dll
vcpkg_install_msbuild(
    SOURCE_PATH "${SOURCE_PATH}"
    INCLUDES_SUBPATH /src/include
    LICENSE_SUBPATH COPYING
    # install_msbuild swaps x86 for win32(bad) if we dont force our own setting
    PLATFORM ${TRIPLET_SYSTEM_ARCH}
    PROJECT_SUBPATH visualc/sharedspice.sln
    TARGET Build
)

if("codemodels" IN_LIST FEATURES)
    # vngspice generates "codemodels" to enhance simulation capabilities
    # we cannot use install_msbuild as they output with ".cm" extensions on purpose
    set(BUILDTREE_PATH "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}")
    file(REMOVE_RECURSE "${BUILDTREE_PATH}")
    file(COPY "${SOURCE_PATH}/" DESTINATION "${BUILDTREE_PATH}")

    if("fftw" IN_LIST FEATURES)
        file(WRITE "${BUILDTREE_PATH}/visualc/Directory.Build.props" "<?xml version=\"1.0\" encoding=\"utf-8\"?>
                                                         <Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
                                                         <ItemDefinitionGroup>
                                                         <ClCompile>
                                                         <AdditionalIncludeDirectories>${CURRENT_INSTALLED_DIR}/include</AdditionalIncludeDirectories>
                                                         </ClCompile>
                                                         <Link>
                                                         <AdditionalLibraryDirectories>${CURRENT_INSTALLED_DIR}/lib;${CURRENT_INSTALLED_DIR}/debug/lib;${CURRENT_INSTALLED_DIR}/lib;${CURRENT_INSTALLED_DIR}/debug/lib</AdditionalLibraryDirectories>
                                                         </Link>
                                                         </ItemDefinitionGroup>
                                                         </Project>")

        vcpkg_build_msbuild(
            PROJECT_PATH "${BUILDTREE_PATH}/visualc/vngspice-fftw.sln"
            # build_msbuild swaps x86 for win32(bad) if we dont force our own setting
            PLATFORM ${TRIPLET_SYSTEM_ARCH}
            TARGET Build
        )
    else()
        vcpkg_build_msbuild(
            PROJECT_PATH "${BUILDTREE_PATH}/visualc/vngspice.sln"
            # build_msbuild swaps x86 for win32(bad) if we dont force our own setting
            PLATFORM ${TRIPLET_SYSTEM_ARCH}
            TARGET Build
        )
    endif()

    # ngspice oddly has solution configs of x64 and x86 but
    # output folders of x64 and win32
    if(VCPKG_TARGET_ARCHITECTURE STREQUAL x64)
        set(OUT_ARCH  x64)
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL x86)
        set(OUT_ARCH  Win32)
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL arm64)
        set(OUT_ARCH  arm64)
    else()
        message(FATAL_ERROR "Unsupported target architecture")
    endif()

    #put the code models in the intended location
    file(GLOB NGSPICE_CODEMODELS_DEBUG
        "${BUILDTREE_PATH}/visualc/codemodels/${OUT_ARCH}/Debug/*.cm"
    )
    file(COPY ${NGSPICE_CODEMODELS_DEBUG} DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib/ngspice")

    file(GLOB NGSPICE_CODEMODELS_RELEASE
        "${BUILDTREE_PATH}/visualc/codemodels/${OUT_ARCH}/Release/*.cm"
    )
    file(COPY ${NGSPICE_CODEMODELS_RELEASE} DESTINATION "${CURRENT_PACKAGES_DIR}/lib/ngspice")


    # copy over spinit (spice init)
    file(RENAME "${BUILDTREE_PATH}/visualc/spinit_all" "${BUILDTREE_PATH}/visualc/spinit")
    file(COPY "${BUILDTREE_PATH}/visualc/spinit" DESTINATION "${CURRENT_PACKAGES_DIR}/share/ngspice")
endif()

vcpkg_copy_pdbs()

# Unforunately install_msbuild isn't able to dual include directories that effectively layer
file(GLOB NGSPICE_INCLUDES "${SOURCE_PATH}/visualc/src/include/ngspice/*")
file(COPY ${NGSPICE_INCLUDES} DESTINATION "${CURRENT_PACKAGES_DIR}/include/ngspice")

# This gets copied by install_msbuild but should not be shared
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/include/cppduals")

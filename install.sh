#! /usr/bin/bash

##################
# Install env for
# TasksManager tool
#
# Dependencies:
#   - gcc
#   - make
##################

###################
# Colors
###################
RED="\e[31m"
GREEN="\e[32m"
BLUE="\e[34m"
RESET="\e[0m"

##################
# Global vars
##################
SCRIPT=$(basename "${0}")
SCRIPT_PATH=$(dirname $(readlink -f "${0}"))
HOME_DIR="${HOME}/.tasks_manager"
MAN_DIR="${HOME_DIR}/man_pages"
INFO_DIR="${SCRIPT_PATH}/man_pages"
FUNCS_FILE="tasksmanager_alias"
FUNCS_DIR="${HOME_DIR}/custom_functions"
FUNCS_FILE_PATH="${SCRIPT_PATH}/custom_functions/${FUNCS_FILE}"
FUNCS_DIR_PATH="${FUNCS_DIR}/${FUNCS_FILE}"
BASHRC_FILE="${HOME}/.bashrc"
MAKE_FILE="${SCRIPT_PATH}/Makefile"
EXEC_FILE="tasksmanager"
EXEC_FILE_PATH="${SCRIPT_PATH}/bin/${EXEC_FILE}"
LOCAL_DIR="${HOME}/.local/bin"
CC="gcc"
MAKE="make"

##################
# Script help funcs
#   - info  - print info mssg
#   - err   - print err mssg
#   - debug - print debug mssg
#   - clean - clean build
##################
function info()
{
    echo -e "${GREEN}[+] $@${RESET}"
}

function err()
{
    echo -e "${RED}[x] $@${RESET}"
}

function debug()
{
    echo -e "${BLUE}[~] $@${RESET}"
}

function clean()
{
    if ! res=$("${MAKE}" clean); then
        err "failed to clean build"
        exit 1
    else
        info "successfully cleaned build"
    fi
}

##################
# Check dependencies
##################
if ! cc_version=$("${CC}" --version 2>&1); then
    err "${CC} not found"
    exit 1
fi
cc_version_n=$(echo "${cc_version}" | head -n 1 | grep -oE '([0-9]+)+(\.)*([0-9])*(\.)*([0-9])*' | head -n 1)

if ! make_version=$("${MAKE}" --version); then
    err "${MAKE} not found"
    exit 1
fi
make_version_n=$(echo "${make_version}" | head -n 1 | grep -oE '([0-9]+)+(\.)*([0-9])*(\.)*([0-9])*' | head -n 1)

info "${CC}: ${cc_version_n}"
info "${MAKE}: ${make_version_n}"

##################
# Create TasksManager
# env
##################
reinstall=""
if [ -d "${HOME_DIR}" ]; then
    debug "home dir \""${HOME_DIR}"\" alredy exists ..."
    read -p "reinstall? [y/n] " reinstall
fi

if [[ -n "${reinstall}" ]] && [[ "${reinstall}" == "n" ]]; then
    debug "exit ..."
    exit 0
fi

if [ -n "${reinstall}" ]; then
    if ! res=$(rm -rf "${HOME_DIR}"); then
        err "failed to remove \""${HOME_DIR}"\""
        exit 1
    else
        info "successfully removed \""${HOME_DIR}"\""
    fi
fi

if ! res=$(mkdir -p "${MAN_DIR}" && mkdir -p "${FUNCS_DIR}"); then
    err "failed to create env dirs"
    exit 1
else
    info "successfully created env dirs"
fi

##################
# Copy info pages
##################
if ! res=$(cp "${INFO_DIR}"/* "${MAN_DIR}"); then
    err "failed to copy info pages"
    exit 1
else
    info "successfully copied info pages"
fi

##################
# Copy custom funcs
##################
if ! res=$(cp "${FUNCS_FILE_PATH}" "${FUNCS_DIR}"); then
    err "failed to copy funcs file \""${FUNCS_FILE_PATH}"\""
    debug "this function aliases are not mandatory, but helpful"
    debug "continue ..."
else
    info "successfully copied funcs file"
fi

if [ ! -f "${BASHRC_FILE}" ]; then
    err ".bashrc file not found"
    debug "this function aliases are not mandatory, but helpful"
    debug "continue ..."
else
    echo "# tasksmanager custom funcs" >> "${BASHRC_FILE}"
    echo ". ${FUNCS_DIR_PATH}" >> "${BASHRC_FILE}"
    info "custom function aliases added to .bashrc"
fi
    
##################
# Create executable
##################
if res=$("${MAKE}" -f "${MAKE_FILE}"); then
    info "successfully created executable"
else
    err "failed to create executable"
fi

create_local=""
if [ ! -d "${LOCAL_DIR}" ]; then
    debug "local dir \""${LOCAL_DIR}"\" does no exist"
    read -p "create? [y/n] " create_local

    if [ "${create_local}" == "y" ]; then
        if ! res=$(mkdir -p "${LOCAL_DIR}"); then
            err "failed to create local dir"
            exit 1
        else
            info "successfully created local dir"
        fi
    else
        debug "local dir not created, executable available at \""${EXEC_FILE_PATH}"\""
        exit 0
    fi
fi

if ! res=$(cp "${EXEC_FILE_PATH}" "${LOCAL_DIR}"); then
    err "failed to copy executable to local dir"
    exit 1
else
    info "successfully installed"
    clean
fi


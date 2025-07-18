#!/bin/bash
set -euo pipefail

### CONFIGURATION ###
DEFAULT_SHELL="/usr/bin/fish"
ESSENTIAL_PACKAGES=(curl wget sudo)
NEW_USER=""
#####################

echo "====== Debian/Ubuntu WSL Setup Script ======"

# Determine if root access is available
SUDO=""
if [ "$(id -u)" -ne 0 ]; then
    SUDO="sudo"
    if ! command -v sudo &>/dev/null; then
        echo "[!] 'sudo' not found. Installing it..."
        su -c "apt update && apt install -y sudo"
    fi
fi

update_system() {
    echo -e "\n[1] Updating system..."
    $SUDO apt update && $SUDO apt upgrade -y
}

install_essentials() {
    echo -e "\n[2] Installing essential packages..."
    for pkg in "${ESSENTIAL_PACKAGES[@]}"; do
        if ! dpkg -s "$pkg" &>/dev/null; then
            echo "   - Installing $pkg"
            $SUDO apt install -y "$pkg"
        else
            echo "   - $pkg already installed"
        fi
    done
}

maybe_create_user() {
    echo -e "\n[3] User Setup"
    read -rp "Create a new user? (y/n): " choice

    if [[ "$choice" =~ ^[Yy]$ ]]; then
        read -rp "Enter new username: " NEW_USER

        if id "$NEW_USER" &>/dev/null; then
            echo "   - User '$NEW_USER' already exists. Skipping creation."
        else
            echo "   - Creating user '$NEW_USER'..."
            $SUDO adduser "$NEW_USER"
            $SUDO usermod -aG sudo "$NEW_USER"
        fi
    else
        read -rp "Enter existing username to use: " EXISTING
        if id "$EXISTING" &>/dev/null; then
            NEW_USER="$EXISTING"
            echo "   - Using existing user '$NEW_USER'"
        else
            echo "   - ERROR: User '$EXISTING' not found."
            exit 1
        fi
    fi

    # Ensure sudo group access is properly configured
    if ! $SUDO grep -q "^%sudo" /etc/sudoers; then
        echo "   - Configuring sudo group access..."
        echo '%sudo ALL=(ALL:ALL) ALL' | $SUDO tee -a /etc/sudoers > /dev/null
    fi
}

install_fish_shell() {
    echo -e "\n[4] Fish Shell Setup"
    read -rp "Install Fish and make it default for '$NEW_USER'? (y/n): " choice

    if [[ "$choice" =~ ^[Yy]$ ]]; then
        if ! command -v fish &>/dev/null; then
            echo "   - Installing Fish shell..."
            $SUDO apt install -y fish
        else
            echo "   - Fish already installed"
        fi

        if ! grep -q "$DEFAULT_SHELL" /etc/shells; then
            echo "   - Adding $DEFAULT_SHELL to /etc/shells"
            echo "$DEFAULT_SHELL" | $SUDO tee -a /etc/shells > /dev/null
        fi

        echo "   - Setting $DEFAULT_SHELL as default for $NEW_USER"
        $SUDO chsh -s "$DEFAULT_SHELL" "$NEW_USER"
    else
        echo "   - Skipping Fish shell setup"
    fi
}

main() {
    update_system
    install_essentials
    maybe_create_user
    install_fish_shell

    echo -e "\n✅ Setup complete!"
    echo "Switch to your user with: su - $NEW_USER"
}

main

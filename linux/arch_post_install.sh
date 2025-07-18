#!/bin/bash

set -euo pipefail

### CONFIGURATION ###
DEFAULT_SHELL="/usr/bin/fish"
ESSENTIAL_PACKAGES=(sudo git) # Add more essentials here
#####################

echo "====== Arch WSL Setup Script ======"

ensure_downloader() {
    if ! command -v curl &>/dev/null && ! command -v wget &>/dev/null; then
        echo "[!] Neither curl nor wget is installed. Installing curl..."
        sudo pacman -Syu --noconfirm curl wget
    fi
}

# Function to update the system
update_system() {
    echo -e "\n[1] Updating system..."
    sudo pacman -Syu --noconfirm
}

# Function to install essential base packages
install_essentials() {
    echo -e "\n[2] Installing essential packages: ${ESSENTIAL_PACKAGES[*]}"
    for pkg in "${ESSENTIAL_PACKAGES[@]}"; do
        if ! pacman -Qi "$pkg" &>/dev/null; then
            echo "   - Installing $pkg"
            sudo pacman -S --noconfirm "$pkg"
        else
            echo "   - $pkg already installed"
        fi
    done
}

# Function to add a new user interactively
create_user() {
    echo -e "\n[3] Creating a new user"
    read -rp "Enter the username to create: " NEW_USER

    if id "$NEW_USER" &>/dev/null; then
        echo "   - User '$NEW_USER' already exists. Skipping creation."
    else
        echo "   - Creating user '$NEW_USER'..."
        sudo useradd -m -G wheel "$NEW_USER"
        echo "   - Set a password for '$NEW_USER':"
        sudo passwd "$NEW_USER"
    fi
}

# Function to install fish shell and set as default
install_fish_shell() {
    echo -e "\n[4] Optional: Install Fish shell and set as default"
    read -rp "Do you want to install Fish shell and make it default for the new user? (y/n): " choice
    if [[ "$choice" =~ ^[Yy]$ ]]; then
        if ! pacman -Qi fish &>/dev/null; then
            echo "   - Installing fish..."
            sudo pacman -S --noconfirm fish
        else
            echo "   - Fish already installed"
        fi

        # Add to /etc/shells if not present
        if ! grep -q "$DEFAULT_SHELL" /etc/shells; then
            echo "   - Adding $DEFAULT_SHELL to /etc/shells"
            echo "$DEFAULT_SHELL" | sudo tee -a /etc/shells
        fi

        echo "   - Setting $DEFAULT_SHELL as default shell for $NEW_USER"
        sudo chsh -s "$DEFAULT_SHELL" "$NEW_USER"
    else
        echo "   - Skipping fish installation"
    fi
}

# Entry point
main() {
    ensure_downloader
    update_system
    install_essentials
    create_user
    install_fish_shell

    echo -e "\n✅ Setup complete! You can now switch to the user with:"
    echo "   su - $NEW_USER"
}

main

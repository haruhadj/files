#!/bin/bash
set -euo pipefail

### CONFIGURATION ###
DEFAULT_SHELL="/usr/bin/fish"
ESSENTIAL_PACKAGES=(curl wget sudo)
NEW_USER=""  # declare early to avoid unbound variable issues
#####################

echo "====== Debian/Ubuntu WSL Setup Script ======"

# Detect if script is run as root or not
if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# Ensure sudo is available if needed
if [ "$SUDO" = "sudo" ] && ! command -v sudo &>/dev/null; then
    echo "[!] 'sudo' is not installed. Installing..."
    su -c "apt update && apt install -y sudo"
fi

# Function to update the system
update_system() {
    echo -e "\n[1] Updating system..."
    $SUDO apt update && $SUDO apt upgrade -y
}

# Install essential packages
install_essentials() {
    echo -e "\n[2] Installing essential packages: ${ESSENTIAL_PACKAGES[*]}"
    for pkg in "${ESSENTIAL_PACKAGES[@]}"; do
        if ! dpkg -s "$pkg" &>/dev/null; then
            echo "   - Installing $pkg"
            $SUDO apt install -y "$pkg"
        else
            echo "   - $pkg already installed"
        fi
    done
}

# Prompt and optionally create user
maybe_create_user() {
    echo -e "\n[3] User Setup"
    read -rp "Do you want to create a new user? (y/n): " create_choice

    if [[ "$create_choice" =~ ^[Yy]$ ]]; then
        read -rp "Enter the username to create: " input_user
        NEW_USER="$input_user"

        if id "$NEW_USER" &>/dev/null; then
            echo "   - User '$NEW_USER' already exists. Skipping creation."
        else
            echo "   - Creating user '$NEW_USER' and adding to sudo group..."
            $SUDO adduser "$NEW_USER"
            $SUDO usermod -aG sudo "$NEW_USER"

            echo "   - Verifying sudo access for '$NEW_USER'..."
            if ! $SUDO grep -q "^%sudo" /etc/sudoers; then
                echo "   - Updating /etc/sudoers to allow sudo group access..."
                $SUDO bash -c "echo '%sudo ALL=(ALL:ALL) ALL' >> /etc/sudoers"
            fi
        fi
    else
        read -rp "Enter the existing username you want to use: " input_user
        if id "$input_user" &>/dev/null; then
            NEW_USER="$input_user"
            echo "   - Using existing user '$NEW_USER'"
        else
            echo "   - ERROR: User '$input_user' does not exist. Exiting."
            exit 1
        fi
    fi
}

# Optionally install fish and make it default shell
install_fish_shell() {
    echo -e "\n[4] Optional: Install Fish shell and set as default"
    read -rp "Do you want to install Fish shell and make it default for the user '$NEW_USER'? (y/n): " choice
    if [[ "$choice" =~ ^[Yy]$ ]]; then
        if ! command -v fish &>/dev/null; then
            echo "   - Installing fish..."
            $SUDO apt install -y fish
        else
            echo "   - Fish already installed"
        fi

        if ! grep -q "$DEFAULT_SHELL" /etc/shells; then
            echo "   - Adding $DEFAULT_SHELL to /etc/shells"
            echo "$DEFAULT_SHELL" | $SUDO tee -a /etc/shells
        fi

        if [[ -z "$NEW_USER" ]]; then
            echo "   - ERROR: NEW_USER is not set. Cannot set default shell."
            return
        fi

        echo "   - Setting $DEFAULT_SHELL as default shell for $NEW_USER"
        $SUDO chsh -s "$DEFAULT_SHELL" "$NEW_USER"
    else
        echo "   - Skipping fish installation"
    fi
}

# Main entry point
main() {
    update_system
    install_essentials
    maybe_create_user
    install_fish_shell

    echo -e "\n✅ Setup complete!"
    echo "You can now switch to the user with: su - $NEW_USER"
}

main

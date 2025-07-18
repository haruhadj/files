#!/bin/bash
set -euo pipefail

### CONFIGURATION ###
DEFAULT_SHELL="/usr/bin/fish"
ESSENTIAL_PACKAGES=(curl wget sudo)
#####################

echo "====== Arch WSL Setup Script ======"

# Detect if script is run as root or not
if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# Ensure sudo is available if needed
if [ "$SUDO" = "sudo" ] && ! command -v sudo &>/dev/null; then
    echo "[!] 'sudo' is not installed. Installing..."
    pacman -Sy --noconfirm sudo
fi

# Function to update the system
update_system() {
    echo -e "\n[1] Updating system..."
    $SUDO pacman -Syu --noconfirm
}

# Install essential packages
install_essentials() {
    echo -e "\n[2] Installing essential packages: ${ESSENTIAL_PACKAGES[*]}"
    for pkg in "${ESSENTIAL_PACKAGES[@]}"; do
        if ! pacman -Qi "$pkg" &>/dev/null; then
            echo "   - Installing $pkg"
            $SUDO pacman -S --noconfirm "$pkg"
        else
            echo "   - $pkg already installed"
        fi
    done
}

# Create user interactively
create_user() {
    echo -e "\n[3] User Creation Step"
    read -rp "Do you want to create a new user? [y/N]: " CONFIRM
    [[ "$CONFIRM" =~ ^[Yy]$ ]] || { echo "   - Skipping user creation."; return; }

    read -rp "Enter the username to create: " NEW_USER

    if id "$NEW_USER" &>/dev/null; then
        echo "   - User '$NEW_USER' already exists. Skipping creation."
    else
        echo "   - Creating user '$NEW_USER' and adding to wheel group..."
        $SUDO useradd -m -s /bin/bash -G wheel "$NEW_USER"
        echo "   - Set a password for '$NEW_USER':"
        $SUDO passwd "$NEW_USER"

        echo "   - Verifying sudo access for '$NEW_USER'..."
        if ! $SUDO grep -q "^%wheel ALL=(ALL:ALL) ALL" /etc/sudoers; then
            echo "   - Updating /etc/sudoers to allow wheel group sudo access..."
            $SUDO bash -c "echo '%wheel ALL=(ALL:ALL) ALL' >> /etc/sudoers"
        fi
    fi
}

# Optionally install fish and make it default shell
install_fish_shell() {
    echo -e "\n[4] Optional: Install Fish shell and set as default"
    read -rp "Do you want to install Fish shell and make it default for the new user? (y/n): " choice
    if [[ "$choice" =~ ^[Yy]$ ]]; then
        if ! pacman -Qi fish &>/dev/null; then
            echo "   - Installing fish..."
            $SUDO pacman -S --noconfirm fish
        else
            echo "   - Fish already installed"
        fi

        if ! grep -q "$DEFAULT_SHELL" /etc/shells; then
            echo "   - Adding $DEFAULT_SHELL to /etc/shells"
            echo "$DEFAULT_SHELL" | $SUDO tee -a /etc/shells
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
    create_user
    install_fish_shell

    echo -e "\n✅ Setup complete!"
    echo "You can now switch to the user with: su - $NEW_USER"
}

main

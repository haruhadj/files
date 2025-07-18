#!/bin/bash

# Check for fish shell path
FISH_PATH="/usr/sbin/fish"

# Install fish if it's not found
if ! command -v fish &>/dev/null; then
  echo "Fish shell not found. Installing..."
  if [ -x "$(command -v apt)" ]; then
    sudo apt update && sudo apt install -y fish
  elif [ -x "$(command -v dnf)" ]; then
    sudo dnf install -y fish
  elif [ -x "$(command -v pacman)" ]; then
    sudo pacman -Sy fish --noconfirm
  else
    echo "Package manager not detected. Please install fish manually."
    exit 1
  fi
fi

# Add fish to /etc/shells if not already present
if ! grep -q "$FISH_PATH" /etc/shells; then
  echo "Adding $FISH_PATH to /etc/shells"
  echo "$FISH_PATH" | sudo tee -a /etc/shells
fi

# Change default shell to fish
echo "Changing default shell to $FISH_PATH"
chsh -s "$FISH_PATH"

# Confirm result
echo "Default shell changed. You may need to log out and back in."
echo "Current shell is: $SHELL"

#!/bin/bash

# Default base path
BASE_PATH="$HOME/react/projects"

# Ask for project name if not provided
if [ -z "$1" ]; then
  read -rp "Enter project name: " PROJECT_NAME
else
  PROJECT_NAME="$1"
fi

# Ask user: JS or TS
echo "Choose project type:"
echo " [1] TypeScript (TSX)"
echo " [2] JavaScript (JSX)"
read -rp "Your choice: " choice

case $choice in
  1) TEMPLATE="react-ts"; CONFIG_FILE="vite.config.ts" ;;
  2) TEMPLATE="react"; CONFIG_FILE="vite.config.js" ;;
  *) echo "⚠ Invalid choice, defaulting to TypeScript."; TEMPLATE="react-ts"; CONFIG_FILE="vite.config.ts" ;;
esac

PROJECT_PATH="$BASE_PATH/$PROJECT_NAME"

# Create base path if missing
mkdir -p "$BASE_PATH"

echo "🚀 Creating new Vite + React ($TEMPLATE) project at $PROJECT_PATH"

cd "$BASE_PATH" || exit 1
if ! npm create vite@latest "$PROJECT_NAME" -- --template "$TEMPLATE"; then
  echo "❌ Failed to create Vite project"
  exit 1
fi

cd "$PROJECT_PATH" || exit 1

# Install Tailwind
if ! npm install -D tailwindcss @tailwindcss/vite; then
  echo "❌ Failed to install Tailwind"
  exit 1
fi

# --- Update vite.config ---
if [ -f "$CONFIG_FILE" ]; then
  if ! grep -q "@tailwindcss/vite" "$CONFIG_FILE"; then
    sed -i "1i import tailwindcss from '@tailwindcss/vite'" "$CONFIG_FILE"
  fi
  if ! grep -q "tailwindcss()" "$CONFIG_FILE"; then
    sed -i "s/plugins: \[/plugins: [\n    tailwindcss(),/" "$CONFIG_FILE"
  fi
  echo "✅ $CONFIG_FILE updated with Tailwind plugin."
else
  echo "⚠ $CONFIG_FILE not found, skipping plugin injection."
fi

# --- Add Tailwind import in CSS ---
CSS_PATH="src/index.css"
if [ ! -f "$CSS_PATH" ]; then
  mkdir -p src
  echo "@import 'tailwindcss';" > "$CSS_PATH"
elif ! grep -q "@import 'tailwindcss';" "$CSS_PATH"; then
  echo "@import 'tailwindcss';" >> "$CSS_PATH"
fi
echo "✅ Tailwind imported into $CSS_PATH"

# --- Prettier config ---
echo "📦 Installing Prettier and prettier-plugin-tailwindcss..."
npm install -D prettier prettier-plugin-tailwindcss

PRETTIER_CONFIG=".prettierrc"
echo '{ "plugins": ["prettier-plugin-tailwindcss"] }' > "$PRETTIER_CONFIG"
echo "✅ Prettier Tailwind plugin installed and configured!"

# --- Open in VS Code ---
read -rp "📂 Open project in VS Code? (Y/N): " open_code
if [[ "$open_code" =~ ^[Yy]$ ]]; then
  if command -v code >/dev/null 2>&1; then
    code .
  else
    echo "⚠ VS Code 'code' CLI not found in PATH."
  fi
fi

# --- Run dev server ---
read -rp "▶ Run npm run dev now? (Y/N): " run_dev
if [[ "$run_dev" =~ ^[Yy]$ ]]; then
  npm run dev
else
  echo "💡 To start later, run: cd \"$PROJECT_PATH\" && npm run dev -- --host"
fi


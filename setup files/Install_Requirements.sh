#!/bin/bash

# Update package lists
sudo apt-get update

# Read the requirements.txt file and install each package
while read -r package; do
    sudo apt-get install -y "$package"
done < requirements.txt

echo "All dependencies have been installed."


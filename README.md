
# Fuse Filebase Manager

### Overview
The **Fuse Filebase Manager** is a **FUSE-based file system implementation** that provides a virtualized file management system. It enables users to interact with files in a structured and efficient manner, supporting **inode-based storage, directory management, and block-level operations**. This project is designed for users who need a lightweight, **custom file system** that can be mounted and used like a traditional storage system.

### Features
- **FUSE (Filesystem in Userspace) Integration** for flexible file system operations.
- **Inode-based file management** for efficient file indexing and retrieval.
- **Bitmap-based block allocation** to optimize storage usage.
- **Directory management** with hierarchical file structuring.
- **Persistent storage with `nufs.mg`** to retain file data across sessions.
- **Makefile for automated compilation** and setup.

### Project Structure
```
/helpers        # Helper functions for file operations
/mnt            # Mount point for the file system
-f              # Fuse Filebase uploaded files
Archive.zip     # Archive of the project files
Makefile        # Compilation and build script
README.md       # Documentation
bitmap.c/.h     # Bitmap-based block allocation implementation
blocks.c/.h     # Low-level block management
directory.c/.h  # Directory management operations
inode.c/.h      # Inode handling logic
nufs.c          # Main file system implementation
nufs.mg         # Storage file for persistent data
slist.c/.h      # Singly linked list utilities
storage.c/.h    # Storage abstraction layer
test.pl         # Testing script for validation
```

### Installation & Usage

#### Prerequisites
- **Linux/macOS** (FUSE support required)
- **GCC Compiler** (for building the project)
- **FUSE Library** (`libfuse`)

#### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Fuse-Filebase-Manager
   cd Fuse-Filebase-Manager
   ```
2. Install dependencies (if needed):
   ```bash
   sudo apt-get install libfuse-dev   # Debian/Ubuntu
   brew install macfuse               # macOS
   ```
3. Compile the file system:
   ```bash
   make
   ```
4. Run and mount the file system:
   ```bash
   ./nufs -f mnt
   ```
5. Perform file operations:
   ```bash
   cd mnt
   touch testfile.txt
   echo "Hello, Fuse!" > testfile.txt
   cat testfile.txt
   ```
6. Unmount the file system:
   ```bash
   fusermount -u mnt   # Linux
   umount mnt          # macOS
   ```

### Future Improvements
- Implement **journaling** for improved fault tolerance.
- Add **encryption features** for secure file storage.
- Introduce **network file system (NFS) support** for distributed use.


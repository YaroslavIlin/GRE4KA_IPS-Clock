import os
Import("env")
# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)
env.Replace(COMPILATIONDB_PATH=os.path.join("$BUILD_DIR", "compile_commands.json"))

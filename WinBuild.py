import os

script_dir_path = os.path.dirname(os.path.abspath(__file__))
build_path = os.path.join(script_dir_path, "build")

# builds the CMake project and creates the executable.
output_code: int = os.system(f"cmake -S \"{script_dir_path}\" -B \"{build_path}\" -G \"MinGW Makefiles\"")
if output_code == 0:
    os.system(f"cmake --build \"{build_path}\"")

input("press enter to exit...")

from subprocess import Popen, PIPE
from shlex import split
from os import path, makedirs

class Constants:
    IF_PREFIX = "[+]"
    ELSE_PREFIX = "[-]"

    NAME_PROJECT = "Puma"
    NAME_LOGO = "puma.png"

    # Directories
    DIR_DOCS = "docs"
    DIR_HTML = "html"
    DIR_ASSETS = "assets"

    # Relative paths
    PATH_HTML = path.join(DIR_DOCS, DIR_HTML)
    PATH_ASSETS_DOCS = path.join(DIR_DOCS, DIR_ASSETS)
    PATH_ASSETS_HTML = path.join(PATH_HTML, DIR_ASSETS)
    PATH_LOGO_DOCS = path.join(PATH_ASSETS_DOCS, NAME_LOGO)
    PATH_LOGO_HTML = path.join(PATH_ASSETS_HTML, NAME_LOGO)
    PATH_HTML_HEADER = path.join(PATH_HTML, "header.html")
    PATH_DOXYFILE = path.join(DIR_DOCS, "Doxyfile")
    PATH_NGINX_HTML = "/var/www/html"
    PATH_NGINX_PROJECT = path.join(PATH_NGINX_HTML, NAME_PROJECT)
    PATH_INDEX_HTML = "index.html"
    # PATH_INDEX_HTML = path.join(DIR_HTML, "index.html")
    
    DOCS_FULL_URL = path.join(NAME_PROJECT, PATH_INDEX_HTML)


class DoxygenConfigs:

    README_PATH = "README.md"
    INPUT_DIRS=f"src {README_PATH}"
    DOXYFILE_CONFIGS = {
        "PROJECT_NAME": {"My Project": Constants.NAME_PROJECT},
        "PROJECT_LOGO": {"=": f"= {path.join(Constants.DIR_DOCS, Constants.NAME_LOGO)}"},
        "INPUT": {"=": f"= {INPUT_DIRS}"},
        "OUTPUT_DIRECTORY": {"=": f"= {Constants.DIR_DOCS}"},
        "RECURSIVE": {"NO": "YES"},
        "EXTRACT_ALL": {"NO": "YES"},
        "GENERATE_LATEX": {"YES": "NO"},
        "GENERATE_HTML": {"NO": "YES"},
        "USE_MDFILE_AS_MAINPAGE": {"=": f"= {README_PATH}"}
    }


def run_bash_command(cmd: str, show_output: bool=False) -> None:
    try:
        # Split the command using shlex to safely parse the command
        command_list = split(cmd)
        # Use Popen to execute the command without shell=True
        process = Popen(command_list, stdout=PIPE, stderr=PIPE, text=True)
        stdout, stderr = process.communicate()

        # Check the return code of the process
        if process.returncode != 0 and show_output:
            print(f"Error: Command failed with code {process.returncode}")
            print(f"stderr: {stderr}")
        
        if show_output:
            print(stdout)

        return stdout

    except Exception as e:
        print(f"An error occurred: {str(e)}")


def install_package(package: str, show_output: bool=False) -> None:
    if run_bash_command(f"dpkg -l | grep -w {package}"):
        print(f"{Constants.ELSE_PREFIX} {package} is already installed")
    
    else:
        run_bash_command("sudo apt update", show_output=show_output)
        run_bash_command(f"sudo apt install {package} -y", show_output=show_output)


def edit_file_lines(file: str, configs: dict) -> None:
    try:
        with open(file, 'r') as f:
            lines = f.readlines()
        
        for index, line in enumerate(lines):
            for target_line, new_line in configs.items():
                
                # Skip commented lines
                if line.strip().startswith('#'):
                    continue

                if target_line in line.split():
                    old_value = list(new_line.keys())[0]
                    new_value = list(new_line.values())[0]

                    # Skip if new value already exists
                    if new_value in line:
                        continue

                    print(f"{Constants.IF_PREFIX} Replacing: '{old_value}' with '{new_value}' in line {line}")
                    lines[index] = lines[index].replace(old_value, new_value)
                
        with open(file, 'w') as f:
            f.writelines(lines)

    except Exception as e:
        print(f"An error occurred: {str(e)}")

def main() -> None:

    print("=====================================================")
    print(f"       Generating {Constants.NAME_PROJECT} Documentation      ")
    print("=====================================================")

    # 1. Validate needed packages
    install_package(package="doxygen", show_output=True)
    install_package(package="graphviz", show_output=True)
    install_package(package="nginx", show_output=True)

    # Validate directories
    makedirs(Constants.DIR_DOCS, exist_ok=True)
    makedirs(Constants.PATH_HTML, exist_ok=True)

    # # Copy needed assets
    run_bash_command(f"cp {Constants.PATH_LOGO_DOCS} {Constants.DIR_DOCS}", show_output=True)

    # Create Doxygen configuration file if not already present
    if not path.exists(Constants.PATH_DOXYFILE):
        print(f"{Constants.IF_PREFIX} Generating Doxygen configuration file...")
        run_bash_command(f"doxygen -g {Constants.PATH_DOXYFILE}", show_output=True)
    else:
        print("Doxygen configuration file already exists.")

    # Configure Doxyfile
    edit_file_lines(Constants.PATH_DOXYFILE, DoxygenConfigs.DOXYFILE_CONFIGS)

    # Generate Doxygen documentation
    print(f"{Constants.IF_PREFIX} Generating documentation for {Constants.NAME_PROJECT}...")
    run_bash_command(f"doxygen {Constants.PATH_DOXYFILE}", show_output=True)

    # Start nginx service
    print(f"{Constants.IF_PREFIX} Starting nginx...")
    run_bash_command("service nginx start", show_output=True)

    # Copy the documentation to nginx
    print(f"{Constants.IF_PREFIX} Copying documentation to {Constants.PATH_NGINX_PROJECT}")
    run_bash_command(f"sudo cp -r {Constants.PATH_HTML} {Constants.PATH_NGINX_PROJECT}")
    print(f"{Constants.IF_PREFIX} Documentation for project '{Constants.NAME_PROJECT}' is now available at http://localhost/{Constants.DOCS_FULL_URL}")

if __name__ == "__main__":
    main()


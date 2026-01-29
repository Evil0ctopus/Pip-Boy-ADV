#!/usr/bin/env python3
"""
Auto-increment firmware version on each build
Reads version.txt, increments patch version, updates header file
"""
Import("env")
import os

def increment_version(source, target, env):
    """Increment the patch version number"""
    version_file = os.path.join(env['PROJECT_DIR'], 'version.txt')
    header_file = os.path.join(env['PROJECT_DIR'], 'include', 'version.h')
    
    # Read current version
    try:
        with open(version_file, 'r') as f:
            version = f.read().strip()
    except:
        version = '1.0.0'
    
    # Parse version
    parts = version.split('.')
    major, minor, patch = int(parts[0]), int(parts[1]), int(parts[2])
    
    # Increment patch version
    patch += 1
    new_version = f"{major}.{minor}.{patch}"
    
    # Write new version
    with open(version_file, 'w') as f:
        f.write(new_version + '\n')
    
    # Get build date/time
    from datetime import datetime
    build_date = datetime.now().strftime('%Y-%m-%d')
    build_time = datetime.now().strftime('%H:%M:%S')
    
    # Generate version header
    header_content = f"""// Auto-generated version file - DO NOT EDIT MANUALLY
#ifndef VERSION_H
#define VERSION_H

#define FIRMWARE_VERSION "{new_version}"
#define FIRMWARE_VERSION_MAJOR {major}
#define FIRMWARE_VERSION_MINOR {minor}
#define FIRMWARE_VERSION_PATCH {patch}
#define BUILD_DATE "{build_date}"
#define BUILD_TIME "{build_time}"
#define BUILD_TIMESTAMP "{build_date} {build_time}"

#endif // VERSION_H
"""
    
    with open(header_file, 'w') as f:
        f.write(header_content)
    
    print(f"Version updated: {version} -> {new_version}")
    print(f"Build timestamp: {build_date} {build_time}")

# Add pre-build action
env.AddPreAction("buildprog", increment_version)

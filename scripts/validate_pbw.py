#!/usr/bin/env python3
"""
validate_pbw.py
Validates that built Pebble PBW bundles meet all Pebble and Rebble Appstore requirements.
"""

import sys
import os
import zipfile
import json

APPS = ["watchface", "chrono", "timer", "alarm"]

def validate_single_pbw(pbw_path):
    if not os.path.exists(pbw_path):
        print(f"❌ Error: PBW file not found at {pbw_path}")
        return False
        
    print(f"\n🔍 Validating PBW: {pbw_path} ({os.path.getsize(pbw_path):,} bytes)")
    
    try:
        with zipfile.ZipFile(pbw_path, 'r') as z:
            file_list = z.namelist()
            print(f"📦 Archive contents ({len(file_list)} entries)")
                
            # 1. Validate appinfo.json
            if 'appinfo.json' not in file_list:
                print("❌ Error: Missing 'appinfo.json' in root of PBW!")
                return False
                
            appinfo_data = z.read('appinfo.json').decode('utf-8')
            appinfo = json.loads(appinfo_data)
            
            # Required fields for Rebble Appstore
            required_fields = ['uuid', 'displayName', 'targetPlatforms', 'resources']
            for field in required_fields:
                if field not in appinfo:
                    print(f"❌ Error: Missing required field '{field}' in appinfo.json")
                    return False
                    
            print(f"✓ {appinfo['displayName']} (UUID: {appinfo['uuid']})")
            print(f"✓ Platforms: {', '.join(appinfo.get('targetPlatforms', []))}")
            
            # 2. Check platform binaries
            expected_platforms = appinfo.get('targetPlatforms', ['emery', 'basalt', 'diorite', 'aplite'])
            for plat in expected_platforms:
                found = any(plat in f and f.endswith('.bin') for f in file_list) or 'pebble-app.bin' in file_list
                if not found:
                    print(f"⚠️ Warning: Binary for platform '{plat}' not explicitly found in archive.")
                else:
                    print(f"✓ Found binary for platform: {plat}")
                    
            # 3. Check JS app (Clay config)
            if 'pebble-js-app.js' in file_list:
                print("✓ Found PKJS / Clay bundle: pebble-js-app.js")
                
            print("🎉 PBW Validation Passed Successfully!")
            return True
            
    except Exception as e:
        print(f"❌ PBW Validation Failed: {e}")
        return False

def main():
    if len(sys.argv) > 1:
        success = validate_single_pbw(sys.argv[1])
        sys.exit(0 if success else 1)
    
    # Validate all dist PBWs
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    dist_dir = os.path.join(repo_root, "dist")
    all_ok = True
    for app in APPS:
        pbw_path = os.path.join(dist_dir, f"pebble-pulsar-{app}.pbw")
        if not validate_single_pbw(pbw_path):
            all_ok = False
            
    sys.exit(0 if all_ok else 1)

if __name__ == '__main__':
    main()

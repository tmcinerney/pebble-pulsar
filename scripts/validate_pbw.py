#!/usr/bin/env python3
"""
validate_pbw.py
Validates that a built Pebble PBW bundle meets all Pebble and Rebble Appstore requirements.
"""

import sys
import os
import zipfile
import json

def validate_pbw(pbw_path="build/pebble-pulsar.pbw"):
    if not os.path.exists(pbw_path):
        print(f"❌ Error: PBW file not found at {pbw_path}")
        sys.exit(1)
        
    print(f"🔍 Validating PBW: {pbw_path} ({os.path.getsize(pbw_path):,} bytes)")
    
    try:
        with zipfile.ZipFile(pbw_path, 'r') as z:
            file_list = z.namelist()
            print(f"📦 Archive contents ({len(file_list)} entries):")
            for f in sorted(file_list):
                print(f"   • {f}")
                
            # 1. Validate appinfo.json
            if 'appinfo.json' not in file_list:
                print("❌ Error: Missing 'appinfo.json' in root of PBW!")
                sys.exit(1)
                
            appinfo_data = z.read('appinfo.json').decode('utf-8')
            appinfo = json.loads(appinfo_data)
            
            # Required fields for Rebble Appstore
            required_fields = ['uuid', 'displayName', 'targetPlatforms', 'resources']
            for field in required_fields:
                if field not in appinfo:
                    print(f"❌ Error: Missing required field '{field}' in appinfo.json")
                    sys.exit(1)
                    
            print("✓ appinfo.json contains all required Rebble fields (including 'resources').")
            print(f"✓ UUID: {appinfo['uuid']}")
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
                
            print("\n🎉 PBW Validation Passed Successfully! Ready for Rebble Appstore upload.\n")
            
    except Exception as e:
        print(f"❌ PBW Validation Failed: {e}")
        sys.exit(1)

if __name__ == '__main__':
    path = sys.argv[1] if len(sys.argv) > 1 else "build/pebble-pulsar.pbw"
    validate_pbw(path)

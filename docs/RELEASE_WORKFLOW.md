# Pebble Pulsar Suite — Independent Release Workflow

This document codifies the release and versioning lifecycle for the **Pebble Pulsar Monorepo Suite**.

---

## 📌 Independent Versioning Architecture

Each application in the suite is published independently to the [Rebble Appstore](https://dev-portal.rebble.io/) and maintains its own semantic version:

| Application | Monorepo Path | Package Key | Current Version | Release Tag Pattern |
| :--- | :--- | :--- | :--- | :--- |
| **Pulsar 1970 Watchface** | `apps/watchface/` | `pebble-pulsar-watchface` | `v2.2.0` | `watchface-v*` |
| **Pulsar Chronograph** | `apps/chrono/` | `pebble-pulsar-chrono` | `v1.1.0` | `chrono-v*` |
| **Pulsar Countdown Timer** | `apps/timer/` | `pebble-pulsar-timer` | `v1.0.0` | `timer-v*` |
| **Pulsar Multi-Alarm Clock** | `apps/alarm/` | `pebble-pulsar-alarm` | `v1.0.0` | `alarm-v*` |
| **Entire Suite (Coordinated)** | Repository Root | `pebble-pulsar-suite` | `v2.2.0` | `suite-v*` or `v*` |

---

## 🚀 Release Process (Step-by-Step)

When shipping a new version of an app (e.g. `chrono`):

### 1. Update Version in `package.json`
Edit `apps/chrono/package.json`:
```json
{
  "name": "pebble-pulsar-chrono",
  "version": "1.1.0",
  "versionLabel": "1.1.0"
}
```

### 2. Update the App's `CHANGELOG.md`
Add a new release section to `apps/chrono/CHANGELOG.md` following [Keep a Changelog](https://keepachangelog.com/) standards:
```markdown
## [1.1.0] - 2026-08-23
### Added
- Live lap split freeze (3s hold on UP button).
- Interactive 20-lap review browser when stopped.
- Best lap delta indicator.
- Automatic hours scaling past 60 minutes.
```

### 3. Generate & Validate Screenshots
Run the automated asset pipeline to refresh the app's screenshots and 3D device mockups:
```bash
npm run assets:chrono
# Or:
devenv shell -- python3 scripts/generate_app_assets.py chrono
```

### 4. Run Verification Tests & Build Bundle
```bash
npm test
npm run build:chrono
python3 scripts/validate_pbw.py
```

### 5. Commit, Tag, and Push
Commit the changes using **Conventional Commits**, create an annotated Git tag matching the app's release pattern, and push:
```bash
git add apps/chrono/ package.json screenshots/chrono/ screenshots/mockups/chrono/
git commit -m "chore(chrono): release v1.1.0"
git tag -a chrono-v1.1.0 -m "Release Pulsar Chronograph v1.1.0"

git push origin main
git push origin chrono-v1.1.0
```

---

## ⚡ GitHub Actions Automation

Pushing any tag matching `<app>-v*` triggers [`.github/workflows/release.yml`](../.github/workflows/release.yml):

1. **Tag Metadata Parsing:** Extracts the target app name (`chrono`) and version string (`1.1.0`).
2. **Targeted Compilation:** Builds **only** the specified app's `.pbw` bundle across all target architectures (`emery`, `basalt`, `diorite`, `aplite`).
3. **Artifact Validation:** Executes `validate_pbw.py` to confirm binary headers and platform packs.
4. **GitHub Release Publication:** Creates a GitHub Release titled e.g. **`Pulsar Chronograph v1.1.0`** with `pebble-pulsar-chrono.pbw` attached and automated release notes.

---

## 🛠️ Monorepo Quick Commands

| Action | Command |
| :--- | :--- |
| **Build Entire Suite** | `npm run build` |
| **Build Single App** | `npm run build:watchface`<br>`npm run build:chrono`<br>`npm run build:timer`<br>`npm run build:alarm` |
| **Run Parity Tests** | `npm test` |
| **Generate & Validate Assets** | `npm run assets:watchface`<br>`npm run assets:chrono`<br>`npm run assets:timer`<br>`npm run assets:alarm`<br>`npm run assets:all` |
| **Validate PBWs** | `npm run validate:pbw` |

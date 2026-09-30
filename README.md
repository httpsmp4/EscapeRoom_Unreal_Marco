# EscapeRoom_Unreal

Progetto Unreal Engine **5.6** per l'Escape Room in VR.

Nasce come clone del progetto `ViveTracker` (`VR_Games_Headless.uproject`), di cui mantiene Blueprint, contenuti e impostazioni; cambia solo il nome del progetto (`EscapeRoom_Unreal.uproject`).

## Requisiti

- Unreal Engine 5.6
- Visual Studio 2022 con i componenti elencati in `.vsconfig` (workload *Game development with C++*, MSVC 14.38, Windows 11 SDK 22621)
- [Git LFS](https://git-lfs.com/)
- Runtime OpenXR (es. SteamVR) per visore e Vive Tracker

## Primo avvio

```bash
git lfs install
git clone https://github.com/httpsmp4/EscapeRoom_Unreal_Marco.git
```

1. Click destro su `EscapeRoom_Unreal.uproject` → *Generate Visual Studio project files*.
2. Apri `EscapeRoom_Unreal.uproject`: al primo avvio l'editor chiede di compilare il modulo C++, rispondi *Yes* (oppure compila il target `VR_Games_HeadlessEditor` da Visual Studio).

## Struttura

| Cartella | Contenuto |
| --- | --- |
| `Config/` | Impostazioni del progetto (`DefaultEngine.ini`, `DefaultInput.ini`, ...) |
| `Content/` | Asset, mappe e Blueprint |
| `Source/VR_Games_Headless/` | Modulo C++ di gioco (widget dei puzzle `CPPW_*`) |
| `Samples/PixelStreaming/` | Web server di esempio per Pixel Streaming |

### Nota sul nome del modulo C++

Il modulo C++ e i target si chiamano ancora `VR_Games_Headless` di proposito: i Blueprint referenziano le classi native come `/Script/VR_Games_Headless.<Classe>`, quindi rinominare il modulo li romperebbe (servirebbero dei Core Redirect in `DefaultEngine.ini`). Il nome del progetto è dato dal solo file `.uproject`.

## Plugin abilitati

OpenXR, OpenXRViveTracker, LiveLink, LiveLinkXR, PixelStreaming, ModelingToolsEditorMode.

## Git LFS

I file binari sono gestiti con Git LFS secondo le regole in `.gitattributes`: asset Unreal (`*.uasset`, `*.umap`), modelli 3D (`*.fbx`, ...), texture sorgente, audio/video e archivi.

`Content/` pesa circa 30 GB: verifica la quota LFS disponibile sul remote prima del push.

# museLuxe Music Assistant / Sendspin

Firmware ESPHome pour la RASPIAUDIO museLuxe, utilisable comme lecteur
synchronisé Music Assistant grâce au protocole Sendspin.

## Compatibilité

- ESPHome 2026.5.0 ou plus récent ;
- Home Assistant avec l'intégration ESPHome ;
- Music Assistant avec le fournisseur Sendspin.

Sendspin, `speaker_source`, `audio_file`, le mixer et le resampler utilisés par
`luxe_music.yaml` sont les composants natifs d'ESPHome. Le seul composant
externe conservé est l'initialisation ES8388 de RASPIAUDIO, nécessaire aux
sorties différentielles et à l'horloge MCLK GPIO0 de la museLuxe. Sa révision est
épinglée dans le YAML pour rendre les compilations reproductibles.

## Commandes de la Luxe

- bouton principal : lecture/pause du groupe Sendspin ;
- boutons `Volume Up` et `Volume Down` : volume du groupe Music Assistant ;
- LED verte pendant la lecture, rouge à l'arrêt.

Le lecteur continue de fonctionner si Home Assistant est momentanément absent.
Le Wi-Fi fonctionne sans économie d'énergie afin de limiter les coupures audio.

## Validation, compilation et installation

```bash
esphome config luxe_music.yaml
esphome compile luxe_music.yaml
esphome run luxe_music.yaml --device /dev/ttyUSB0
```

La configuration de test actuelle est versionnée `2026.8.3`. Ne pas remplacer
`update_firmware.bin` ni modifier `manifest_update.json` avant validation sur le
matériel.

## Publication après validation

1. Compiler `luxe_music.yaml`.
2. Copier `.esphome/build/muse-luxe-music/build/firmware.ota.bin` vers
   `update_firmware.bin`.
3. Calculer `md5sum update_firmware.bin`.
4. Reporter la version et le MD5 dans `manifest_update.json`.

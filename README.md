# museLuxe Music Assistant / Sendspin

Firmware ESPHome pour la RASPIAUDIO museLuxe, utilisable comme lecteur
synchronisé Music Assistant grâce au protocole Sendspin.

## Compatibilité

- ESPHome 2026.8.2 ou plus récent ;
- Home Assistant avec l'intégration ESPHome ;
- Music Assistant avec le fournisseur Sendspin.

Sendspin, `speaker_source`, `audio_file`, le mixer et le resampler utilisés par
`luxe_music.yaml` sont les composants natifs d'ESPHome. L'initialisation ES8388
de RASPIAUDIO reste nécessaire aux sorties différentielles et à l'horloge MCLK
GPIO0 de la museLuxe ; sa révision est épinglée dans le YAML. Le composant local
`components/luxe_ip5306/` gère le circuit de charge des Luxe récentes et doit
accompagner `luxe_music.yaml` lors de la distribution des sources.

## Commandes de la Luxe

- bouton `Play or Pause` : lecture/pause du groupe Sendspin ;
- boutons `Volume Up` et `Volume Down` : volume du groupe Music Assistant ;
- LED verte pendant la lecture, rouge à l'arrêt.

Le lecteur continue de fonctionner si Home Assistant est momentanément absent.
Le Wi-Fi fonctionne sans économie d'énergie afin de limiter les coupures audio.

## Batterie

La version `2026.9.3` expose `Battery voltage` en volts,
`Battery level` (pourcentage estimé avec la courbe de la Radio) et
`Battery low` dans Home Assistant. La mesure GPIO33 et son étalonnage restent
ceux de la Luxe : la calibration ADC de la Radio ne convient pas à ce modèle.

Sous le seuil `Low battery threshold` (20 % par défaut), la LED clignote
(500 ms allumée, 500 ms éteinte) dans sa couleur
courante, y compris après un changement lecture/arrêt ou une commande HA.
L'alerte est prioritaire sur les effets manuels et l'extinction de la LED.
À partir du seuil + 5 points, le clignotement cesse et la couleur courante
reste fixe.
Le filtrage de tension évite les alertes dues à des variations brèves ; il
introduit aussi un délai de réaction. Le pourcentage dépend de la batterie,
du volume sonore et de la charge : il doit être vérifié sur le matériel.

Pour valider : tester une batterie faible en lecture puis en pause, modifier
la couleur dans HA et vérifier le retour à une LED fixe après recharge.

Sur les Luxe récentes, le circuit IP5306 ajoute des états de charge et un
interrupteur `Allow charging`. Sur les anciennes, `IP5306 available` reste OFF
et la lecture audio ainsi que la mesure de tension fonctionnent normalement.
Voir [README_IP5306.md](README_IP5306.md) pour le détail des entités et limites.

## Validation, compilation et installation

```bash
esphome config luxe_music.yaml
esphome compile luxe_music.yaml
esphome run luxe_music.yaml --device /dev/ttyUSB0
python3 tests/run_ip5306_tests.py
```

La configuration principale est versionnée `2026.9.3`. La variante IP5306 a été
testée sur des Luxe avec et sans ce circuit. Les noms de certaines entités ont changé ;
vérifier les références à `Battery`, `Button` ou `Signal WiFi museLuxe` dans les
tableaux de bord et automatisations Home Assistant.

## Publication après validation

1. Compiler `luxe_music.yaml`.
2. Copier `.esphome/build/muse-luxe-music/build/firmware.ota.bin` vers
   `update_firmware.bin`.
3. Calculer `md5sum update_firmware.bin`.
4. Reporter la version et le MD5 dans `manifest_update.json`.

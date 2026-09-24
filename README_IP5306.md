# museLuxe — gestion IP5306-I2C

La gestion IP5306 est intégrée dans `luxe_music.yaml`, version `2026.9.3`.
Le composant local `components/luxe_ip5306/` doit accompagner le YAML.
ESPHome 2026.8.2 minimum (version utilisée pour la compilation).

## Détection et compatibilité

Le circuit est interrogé sur le bus existant GPIO18/GPIO23, adresse **0x75
en notation 7 bits** (0xEA/0xEB dans la notation 8 bits du constructeur).
Toutes les 10 secondes, une transaction sans données vérifie l'acquittement,
puis les registres nécessaires sont lus séparément. Un acquittement indique
un périphérique répondant à cette adresse, pas une identification formelle :
le composant suppose le câblage IP5306 de la Luxe.

Aucune réponse peut signifier circuit absent, endormi ou défaut I2C :
`IP5306 available` passe à OFF. Le lecteur et la mesure ADC continuent.
Les états binaires du circuit sont invalidés et ses capteurs numériques
publient une valeur inconnue, jamais un faux 0 %. Une nouvelle tentative
est effectuée au prochain cycle. L'échec du seul registre de jauge 0x78
n'invalide pas les autres données.

## Entités Home Assistant

| Entité | Fonction |
|---|---|
| IP5306 available | Diagnostic de communication |
| Charging | Charge autorisée, bit de charge actif et batterie non pleine |
| Battery full | Indicateur matériel de fin de charge |
| Charging enabled (hardware) | Lecture du bit d'autorisation de charge |
| IP5306 battery level | Jauge indicative 0/25/50/75/100 %, diagnostic |
| IP5306 charge current limit | Consigne décodée en mA, lecture seule ; pas un courant mesuré |
| Allow charging | Switch de configuration, lecture/modification du seul bit 4 de 0x00 |
| Low battery threshold | Réglage local 5–50 %, défaut 20 %, retour à seuil + 5 points |

Les capteurs ADC `Battery voltage`, `Battery level`, l'alerte `Battery low` et la
LED clignotante restent actifs sur toutes les Luxe. Leur estimation est
indépendante de la jauge grossière de l'IP5306.

Le switch ne restaure pas une valeur enregistrée et n'écrit rien au démarrage :
il reflète le circuit. Une commande fait une lecture/modification/écriture,
préserve tous les autres bits et relit les états. Les commandes identiques
à la valeur déjà présente n'écrivent rien. Une commande impossible est rejetée.
Les préférences du seuil logiciel sont, elles, conservées après redémarrage.

Limite de l'entité switch ESPHome : lors d'une perte I2C, son affichage peut
rester sur le dernier état connu. Pour une automatisation, vérifier d'abord
`IP5306 available = ON`, puis utiliser `Charging enabled (hardware)`
pour confirmer le résultat. Ce capteur est invalidé en cas de panne.

Le bit `Battery full` peut être indéterminé juste après la mise sous tension :
attendre des lectures stables avant de déclencher une automatisation de fin de
charge. `Charging = OFF` ne prouve pas que l'USB est débranché.
Les réglages de tension de charge, de boost et de protection ne sont pas exposés.
L'effet de l'interruption de recharge en fonctionnement sur USB reste à vérifier
sur cette révision matérielle.

## Compilation et essai

```bash
python3 tests/run_ip5306_tests.py
python3 -m esphome config luxe_music.yaml
python3 -m esphome compile luxe_music.yaml
# Après identification de la Luxe sur le port :
python3 -m esphome upload luxe_music.yaml --device /dev/ttyUSB0
```

Les binaires sont dans
`.esphome/build/muse-luxe-music/build/`.
Le YAML conserve l'identité réseau/HA de la Luxe et le mécanisme HTTP de mise
à jour publique. Le binaire et le manifeste doivent être publiés ensemble après
leur validation sur le matériel.

Vérifier sur matériel :

1. Détection sur une Luxe récente ; fonctionnement audio sur une ancienne.
2. États sur batterie, puis USB branché, et en fin de charge.
3. OFF/ON de l'autorisation, confirmation par le retour circuit et maintien audio.
4. Redémarrage : lecture du réglage matériel sans écriture automatique.
5. Seuil réglable et conservation de la couleur de la LED pendant le clignotement.

Les tests hôtes compilent le vrai composant contre un bus I2C simulé :
absence/récupération, registres illisibles, jauge inconnue, décodage du courant,
préservation des bits, commandes répétées et erreurs d'écriture/relecture.
Ils ne remplacent pas la validation électrique sur la carte.

## Références

- [Registres Injoinic, copie M5Stack](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/IIC_IP5306_REG_V1.4_cn.pdf)
- [Pilote M5Unified IP5306](https://github.com/m5stack/M5Unified/blob/master/src/utility/power/IP5306_Class.cpp)
- Bibliothèque locale RASPIAUDIO : `../Muse_library/src/museWrover.h`.

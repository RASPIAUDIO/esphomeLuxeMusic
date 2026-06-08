# esphomeLuxeMusic
*esphomeLuxeMusic* is a new player based on the **SendSpin** protocol, enabling *synchronized multiroom playback*.
(**SendSpin** is specific to Music Assistant).

This is a first attempt and is intended to evolve (we welcome your feedback on the *raspiaudio.com* forum).

You can:

**either** compile it directly from *luxe_music.yaml* (note: esphome version 3.1.4+)

**or** load the binary directly from *raspiaudio.github.io*

(**Special thanks to Deco** (Thomas Nonato Beck) for his crucial contribution)













/////////////////////////////////////////////////////////////////////////////////
// reminder UPDATE
/////////////////////////////////////////////////////////////////////////////////
(First Change the project version in the yaml file)
1. .bin ..../.esphome/build/raspiaudio-radio/.pioenvs/raspiaudio-radio/firmware.ota.bin ===> update_firmware.bin
2. calcul parité ==> >> md5sum update_firmware.bin
3. modifier avec le résultat la ligne "md5": de manifest_update.json
/////////////////////////////////////////////////////////////////////////////////

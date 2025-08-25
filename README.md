## -SETTING UP YOUR GAME- ##
!!!!!! WARNING !!!!!
- This requires a legally obtained ISO file of Super Smash Bros. Melee, USA release, disk revision 2
- You MUST turn off all cheats/codes and Retroachievements integration for this to function.
- When you play Melee in Archipelago, it will create a new save file per-seed. However, save*states* are not kept per seed. There are guards in place to prevent the game from breaking or sending locations/items if you load a savestate belonging to vanilla or a different seed, but please keep this in mind.

# ---------Generation---------
1. Install the available ``apworld`` file to the ``custom_worlds`` folder within your Archipelago directory
2. If you do not already have a settings yaml, you can run ``Generate Template Options`` from the Launcher, which will give you a yaml file for Melee (and others) that you can edit as you desire.
3. Once your yaml is edited as necessary, place it in the ``players`` folder and run ``ArchipelagoGenerate.exe`` or what have you.

# ------PLAYING YOUR GAME----
1. Once the game is generated, your game will be available in the zip file located in output, or the website once the multiworld data is uploaded
2. Your patch will be a ``.zip`` file likely named AP_1234567_p1_player.
3. Inside of this zip file will be a ``.xml`` file with the same name. This is your patch. Extract it. Optionally, place the patch in your Dolphin directory in ``/Load/Riivolution``
4. With dolphin loaded, right-click Super Smash Bros. Melee from the games list, and select ``Start with Riivolution patches``. A window should open
5. At the bottom, select ``Open Riivolution XML`` to be prompted for a file. Select your XML patch identified in step 3.  You should see the SSBM Archipelago Mod with some player related data appear.
6. Make sure the mod is ``Enabled``, and click start. If this is your first time launching this seed, you will be prompted to create a save file.
7. In the Archipelago Launcher, open the ``Super Smash Bros. Melee Client``
8. It should connect to Dolphin. At this point you can connect to the Multiworld using the address bar at the top of the client.

# ---------CHANGES FOR THE RANDOMIZER---------
- The rate at which you receive Coins has been multiplied by 10.
- The Tom Nook trophy check requires 100 coins instead of 1,000
- The mute city trophy check requires 100 meters instead of 10,000
- You can not unlock characters or their respective 1-player mode trophies from only playing VS. You can only get them from the intended method 
- Mew and Celebi are unlocked from the start, and the chance of finding them has been reduced to 1/100 instead of 1/251
- Mewtwo's Unlock condition has been reduced from 20 combined hours of VS to 1 combined hour of VS
- The 1,000 KO's and 5,000 KO's for the Landmaster trophy and Score Display checks have been reduced to 10 KO's and 50 KO's respectively
- The lottery pool will never increase from exhausting the current pool

# ---------ADDITIONAL NOTES---------
- When all of your goals are complete, enter the Collection room in order to finish.
- You cannot receive items in the Lottery or Gallery menu for stability
- You can run the command ``/check_goals`` in your client to tell you what goals are required, and if you have completed them or not. It will also show your current trophy data.
- You can run the commands ``/check_characters`` and ``/check_modes`` in your client to view received Characters and Modes respectively
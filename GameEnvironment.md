# Summary #
The test game takes the shape of a classic turn-based strategy game with a top-down view.


# Graphical Representation #
![http://interactive-stories.googlecode.com/svn/trunk/wiki/screen01.png](http://interactive-stories.googlecode.com/svn/trunk/wiki/screen01.png)
## World View ##
On the right side of the above screen capture we can see a portion of the terrain on which the game is played. Here is a list of the different shapes and colors with their corresponding semantics:
  * Blue cross: A group of people that is part of your clan.
  * Red cross: A group of people that is not part of your clan.
  * Golden cell: A unit of terrain that can be mined of gold.
  * Green line: The planned path for groups movement.
  * Blue line: The next plan section currently being planned.
  * Blue cell: A cell traversed by the next plan section currently being planned.
  * Blue wireframe square: Indicated that the group is selected.
  * Orange wireframe rectangle: Your selector.
  * Darkened cells: Terrain that lies outside of your visibility radius.

Note that it is only possible to see enemy groups and golden cells if they are within the visibility radius of one (or more) of your groups.

## Menu Panel ##
On the left side of the screen capture we can see the game menu. It is composed of three main panels and the current turn with an **`<End Turn>`** button to terminate the turn.
### Global Panel ###
This panel indicates the total number of remaining clans and the total population.
### Clan Panel ###
This panel indicates the clan population, the number of groups, the gold owned by the clan, and the clan attributes:
  * Stamina is the amount of actions your groups will dispose of at the beginning of a turn.
  * Attack is the capacity of your groups to reduce enemy groups' population when attacking them.
  * Defense is the capacity of your groups to protect from enemy groups' attacks.
  * Culture defines how much other clans will tend to have a natural positive bias towards your clan.
### Groups Panel ###
This panel indicates how many groups are currently selected and the total selected population.
The **`<Select Subgroup>`** button allows you to create a subgroup from a selected group as explained in the Player Actions section.
The bottom section of this panel offers a list to visualize the remaining stamina of the selected groups. Black indicates what has been used already, red indicates what the currently planned actions will consume if they are executed, and blue is what will remain if the planned actions are executed.

# Player Actions #
## Movement ##
## Creating Subgroups ##
## Interactions ##
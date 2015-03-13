# Problems #
## Population Model ##
Since victory is based on population there must be a way to control the population growth.
If there is a maximum population then how can it be enforced ?
The problem posed by an asymptotic model is that population is separated in groups and therefore grows in groups.  Setting a global asymptote would be inconsistent with groups because it would keep small groups from growing. Setting an asymptote per group would make sense but would not limit the total population because one would only have to create new groups to make the clan population grow. The two possible solution I see from here are:
  1. Have a asymptote per group and limit the number of groups.
  1. Have a fixed population limit.

The fixed population limit makes the gameplay simpler and would allow the player to concentrate on other matters but it poses a new problem: How is killed population replaced/regained ?
There is a possibility of using gold as a way to replace population.

## Urge To Win ##
What is the main motivation for players (real or virtual) to win ? The winning conditions seems to point at a unification of the land. But is that strong enough ? Introducing a mechanism to force the player to win would enhance the motivation behind the game, such a motivation should not however force the player towards a given strategy more than another.
Making the clan population dwindle automatically when lacking resources would be an efficient way to ensure motivation (fight for survival), this may however prevent low-money and long-term strategies.

## Gameplay Balance ##
It is impossible to kill every other clan in a few turns , and it should not be possible federate too many clans too fast. The moral compass system prevents from forming alliances with every clan but since the moral compass can only prevent a limited (and preferably low) number of alliances there is need for another way to slow down federating clans.
  1. Requesting a better stance to federate (more cautious alliances).
  1. Requesting a better stance to remain in a federation (less flexible alliances).
  1. Requiring to have two groups next to each other in order to carry out federation.
  1. Any combination of the above.

## Combat System ##
To decide of the combat system we have to decide which features to integrate:
  * Separation of attack and defense.
  * Possibility of attacking and retracting in the same turn.
  * Possibility of defending or running away.
  * Possibility of making several attacks in the same turn.
  * Possibility of fights to continue over several turns for other players to join the fight.

## Economical Pact ##
How can the economical pact be enforced ? Do we allow for defection ? If yes how do we check for defection ?

# Solutions #
Start the game with a substantial amount of gold.
At every turn the population consumes gold to remain alive.
Set a fixed limit for the population, define a gold price to increase the population, the price could start at a defined minimum each turn and increase according to how much population has been produced in the turn.
When population is killed money goes from the killed to the killer. This gives war a cost and also allows blitzkrieg techniques which breaks the linearity of the gameplay.
Defense and Attack should be carried out by using stamina. This allows all of the proposed features.
Federating should require clans to meet (One clan of the federation sends a delegation).
If this is not enough to prevent over-federation we can then use "cautious" federation (require allied stance).
Economical pact could be enforced through a tribute to pay to attacked clans, and a tribute due by attacking clans. Refusal or impossibility to pay the price will result in a modification of the stances of the concerned clans.

The price of the tributes to attacked clans should be determined according to:
  * Force of the attack (and not losses since improvements in defense capabilities of the clan will change losses.)
  * Percentage of the federation's population (larger clans should pay more).

And the price of the tributes due by attacking clans should be determined according to:
  * Force of the defense (and not attack since improvements in attack capabilities of the clan will change it.)
  * The collected money.
  * Percentage of the federation's population (larger clans should receive more).
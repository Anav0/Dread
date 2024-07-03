#ifndef TESTS_H
#define TESTS_H

#define PRINT_PASS std::cout << std::format("{:85} {:->1}\n", __func__, " PASS")
#define PRINT_FAILED std::cout << std::format("{:85} {:->1}", __func__, " FAIL")

 Armory Fixture_Armory();
 std::vector<BattleGroup> Fixture_TestGroup(Armory* armory);

 void MoraleBroke_ReturnsTrueIfMoraleBelowThreshold();
 void MoraleBroke_ReturnsFalseIfMoraleAboveThreshold();
 void AverageDamageExceedsThreshold_ReturnsFalseIfDamageBelowThreshold();
 void AverageDamageExceedsThreshold_ReturnsTrueIfDamageAboveThreshold();
 void TryToHitTarget_CannotHitSomethingOutOfRange();
 void TryToHitTarget_UsesCorrectAccuracyRating();
 void ApplyModifiers_GetsCorrectModifiers();
 void TryPickingRightAmmunitionForTarget_PickesAmmoThatCorrespondsToTargetArmorAndDomain();

 void RunTests();

#endif TESTS_H
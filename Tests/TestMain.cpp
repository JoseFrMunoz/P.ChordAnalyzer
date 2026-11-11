#include <juce_core/juce_core.h>

/** Ejecuta todos los tests y devuelve 1 si alguno falla (para CTest / GitHub Actions). */
int main()
{

    juce::UnitTestRunner runner;
    runner.setAssertOnFailure (false);
    runner.runAllTests();

    int fallos = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
        fallos += runner.getResult (i)->failures;

    std::cout << (fallos == 0 ? "\nTODOS LOS TESTS PASAN\n" : "\nHAY TESTS QUE FALLAN\n");
    return fallos == 0 ? 0 : 1;
}

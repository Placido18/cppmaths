# Programmes de test
add_executable(tp3 tp3.cpp)
target_link_libraries(tp3 librairie)

add_executable(test_performance test_performance.cpp)
target_link_libraries(test_performance librairie)

# Test de visualisation VTK (TP5 Q3)
add_executable(visual_test visual_test.cpp)
target_link_libraries(visual_test librairie)

# Tests Unitaires GTest (TP5 Q1+Q2)
add_executable(tests test_classes.cpp)
target_link_libraries(tests librairie GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
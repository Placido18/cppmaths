# Programmes de test
add_executable(tp3 tp3.cpp)
target_link_libraries(tp3 librairie)

add_executable(test_performance test_performance.cpp)
target_link_libraries(test_performance librairie)
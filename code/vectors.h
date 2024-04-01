struct VECTOR3D
{
	float values[3];
};

VECTOR3D set_vector(float array[3])
{
    VECTOR3D defined_vector;

    for(int i = 0; i < 3; i++)
    {
        defined_vector.values[i] = array[i];
    };

    return defined_vector;
}

VECTOR3D vec_sum(VECTOR3D vec1, VECTOR3D vec2)
{
    VECTOR3D resulting_vec;
    float result;

    for(int i = 0; i < 3; i++)
    {
        result = vec1.values[i] + vec2.values[i];
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

VECTOR3D vec_MultbyScalar(VECTOR3D vec1, int scalar)
{
    VECTOR3D resulting_vec;
    int result;

    for(int i = 0; i < 3; i++)
    {
        result = vec1.values[i] * scalar;
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

int vec_DotProduct(VECTOR3D vec1, VECTOR3D vec2)
{
    int result = 0;

    for(int i = 0; i < 3; i++)
    {
        result += vec1.values[i] * vec2.values[i];
    };

    return result;
};
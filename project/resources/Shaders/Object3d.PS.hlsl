#include "Object3d.hlsli"

struct Material {
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay; 
};
ConstantBuffer<PointLight> gPointLight : register(b3);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition); // Cameraへの方向を算出
    
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position); // 入射光を計算
    
    float32_t distance = length(gPointLight.position - input.worldPosition); // ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay); // 指数によるコントロール
    
    //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal)); // 入射光の反射ベクトル
    
    // textureのa値が0.5以下のときにPixelを棄却
    if (textureColor.a <= 0.5){
        discard;
    }
    // textureのa値が0のときにPielを棄却
    if (textureColor.a == 0.0){
        discard;
    }
    // output.coloのa値が0のときにPixelを棄却
    if (output.color.a == 0.0){
        discard;
    }
    
    if (gMaterial.enableLighting != 0) { // Lightingする場合  
        ///
        /// DirectionalLight
        ///
        
        // 拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f); // half lambert
        
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        // 鏡面反射
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NdotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NdotH), gMaterial.shininess); // 反射強度
        
        float32_t3 specularDirectionalLight =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        ///
        /// PointLight
        ///
        
        // 拡散反射
        float NdotL_point = dot(normalize(input.normal), -pointLightDirection);
        float cos_point = pow(NdotL_point * 0.5f + 0.5, 2.0f);
        float32_t3 diffusePointLight =
        gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos_point * gPointLight.intensity * factor;

        // 鏡面反射
        float32_t3 halfVector_point = normalize(-pointLightDirection + toEye);
        float NdotH_point = dot(normalize(input.normal), halfVector_point);
        float specularPow_point = pow(saturate(NdotH_point), gMaterial.shininess);
        float32_t3 specularPointLight =
        gPointLight.color.rgb * gPointLight.intensity * specularPow_point * float32_t3(1.0f, 1.0f, 1.0f) * factor;
        
       
        
        // ライティングテスト用
        // 拡散反射+鏡面反射
        output.color.rgb = 
        diffuseDirectionalLight + specularDirectionalLight + // DirectionalLight
        diffusePointLight + specularPointLight // PointLight
        ;
        
        // 基本的にはこっちを適用
        //output.color.rgb = diffuseDirectionalLight; // DirectionalLightの拡散反射のみ
        
        
        // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;
    } else {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}
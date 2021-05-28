

#include "ModelRenderSystem.h"

namespace ModelRenderSystem {
    /*
    ModelRenderSystemArray::ModelRenderSystemArray(void) :
        _drawModels(nullptr)
    {

    }

    ModelRenderSystemArray::ModelRenderSystemArray(const std::function<void(ModelRenderSystemData_t*, size_t)>& drawModels) :
        _drawModels(drawModels)
    {
    }

    void ModelRenderSystemArray::Add(const ModelRenderSystemData_t& data)
    {
        _data[_count++] = data;
    }

    void ModelRenderSystemArray::Render(void) const
    {
        if (_count > 0 && _drawModels != nullptr)
            _drawModels(const_cast<ModelRenderSystemData_t*>(&_data[0]), _count);
    }

    void ModelRenderSystemArray::UpdateColor(const std::function<Color(Entity* entity)>& getColorForEntity)
    {
        if (getColorForEntity == nullptr)
            return;

        for (auto i = 0UL; i < _count; ++i) {
            auto data = _data[i];

            auto entity = (&data)->BaseAnimating->GetBaseEntity();
            if (!entity)
                continue;

            *entity->GetColor() = getColorForEntity(entity);
        }
    }


    DrawModelsParam::DrawModelsParam(const std::function<void(ModelRenderSystemData_t*, size_t)>& drawModel, ModelRenderSystemData_t* systemDataArray, size_t count) :
        _systemDataArray{ systemDataArray },
        _count{ count }
    {
        _arrays[1] = ModelRenderSystemArray(drawModel);
        _arrays[2] = ModelRenderSystemArray(drawModel);
        _arrays[3] = ModelRenderSystemArray(drawModel);
    }

    void DrawModelsParam::RenderArray(int arrayType, const std::function<Color(Entity* entity)>& getColorForEntity)
    {
        auto selectedArray = &_arrays[int(arrayType)];

        selectedArray->UpdateColor(getColorForEntity);
        selectedArray->Render();
    }

    bool DrawModelsParam::BuildArrays(void)
    {
        if (_count <= 0)
            return false;

        auto entityArray = &_arrays[int(1)];
        auto weaponArray = &_arrays[int(2)];
        auto grenadeArray = &_arrays[int(3)];

        for (auto i = 0UL; i < _count; ++i) {
            auto data = _systemDataArray[i];

            auto entity = (&data)->BaseAnimating->GetBaseEntity();
            if (entity->IsValid()) {

                if (entity->IsWeapon()) {
                    weaponArray->Add(data);
                    continue;
                }

                if (entity->IsGrenade()) {
                    grenadeArray->Add(data);
                    continue;
                }
            }

            entityArray->Add(data);
        }

        return true;
    }
    */
}





//typedef void(__thiscall* ModelRenderDrawModels)(void* _this, void* ecx, ModelRenderSystemData_t* pEntities, int nCount, ModelRenderMode_t renderMode, char unknown);
//void __fastcall ModelRenderDrawModels(void* _this, void* ecx, ModelRenderSystemData_t* pEntities, int nCount, ModelRenderMode_t renderMode, char unknown);


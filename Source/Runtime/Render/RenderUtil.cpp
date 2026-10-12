#include "EnginePCH.h"
#include "RenderUtil.h"

#include <set>
#include <sstream>

namespace
{
	// include된 파일도 확인해 hlsli 변경 후 오래된 변형 셰이더를 읽지 않게 한다.
	bool AreShaderSourcesOlder(const fs::path& Path, fs::file_time_type CsoTime, std::set<fs::path>& Visited)
	{
		const fs::path Source = Path.lexically_normal();
		if (!Visited.insert(Source).second) return true;

		std::error_code Ec;
		const auto SourceTime = fs::last_write_time(Source, Ec);
		if (Ec || SourceTime >= CsoTime) return false;

		std::ifstream File(Source);
		if (!File) return false;
		std::string Line;
		while (std::getline(File, Line))
		{
			std::istringstream Tokens(Line);
			char Hash;
			std::string Directive;
			if (!(Tokens >> Hash) || Hash != '#' || !(Tokens >> Directive) || Directive != "include") continue;

			char Delimiter;
			if (!(Tokens >> Delimiter) || (Delimiter != '"' && Delimiter != '<')) return false;
			std::string IncludePath;
			if (!std::getline(Tokens, IncludePath, Delimiter == '"' ? '"' : '>')) return false;
			if (!AreShaderSourcesOlder(Source.parent_path() / IncludePath, CsoTime, Visited)) return false;
		}
		return !File.bad();
	}
}

FShaderByteCode RenderUtil::CompileShader(FString Path, const char* EntryPoint, EShaderType ShaderType, const D3D_SHADER_MACRO* Defines)
{
	const char* Target = (ShaderType == EShaderType::Vertex) ? "vs_5_0" : "ps_5_0";

	uint32 Flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	FShaderByteCode ByteCode;

	std::wstring WPath(Path.begin(), Path.end());

	ComPtr<ID3DBlob> ErrorBlob;

	HRESULT hr = D3DCompileFromFile(WPath.c_str(), Defines , D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint, Target, 0, 0, ByteCode.Blob.GetAddressOf(), ErrorBlob.GetAddressOf());

	if (FAILED(hr))
	{
		if (ErrorBlob)
			OutputDebugStringA((const char*)ErrorBlob->GetBufferPointer());
		else
			OutputDebugStringA(("[Shader] file not found: " + Path + "\n").c_str());

		ByteCode.Blob.Reset();
	}
	return ByteCode;
}

void RenderUtil::SaveByteCode(FString CsoPath, const FShaderByteCode& ByteCode)
{
	fs::create_directories(fs::path(CsoPath).parent_path());
	std::ofstream Out(CsoPath, std::ios::binary);
	if (!Out) { fs::remove(CsoPath); return; }
	Out.write(reinterpret_cast<const char*>(ByteCode.GetData()), ByteCode.GetSize());
}

FShaderByteCode RenderUtil::LoadByteCode(const FString& CsoPath)
{
	FShaderByteCode Result;

	std::ifstream In(CsoPath, std::ios::binary | std::ios::ate);
	if (!In) return Result;

	const std::streamsize Size = In.tellg();
	In.seekg(0);
	if (Size <= 0) return Result;

	ComPtr<ID3DBlob> Blob;
	if (FAILED(D3DCreateBlob(static_cast<SIZE_T>(Size), &Blob))) return Result;

	In.read(reinterpret_cast<char*>(Blob->GetBufferPointer()), Size);
	if (In) Result.Blob = std::move(Blob);
	return Result;
}

FString RenderUtil::GetCsoPath(const FString& ShaderPath, EShaderType ShaderType, const FString& Variant)
{
	const fs::path Rel = fs::relative(ShaderPath, "Resources/Shader");
	const char* Suffix = (ShaderType == EShaderType::Vertex) ? "_VS.cso" : "_PS.cso";

	fs::path Out = fs::path("Intermediate/Shaders") / Rel.parent_path();
	Out /= Rel.stem().generic_string() + "_" + Variant + Suffix;
	return Out.generic_string();
}

// .cso 파일이 최신 컴파일인지 비교한다.
bool RenderUtil::IsCsoUpToDate(const FString& ShaderPath, const FString& CsoPath)
{
	std::error_code Ec;  
	if (!fs::exists(CsoPath, Ec)) return false;

	const auto CsoTime = fs::last_write_time(CsoPath, Ec);
	if (Ec) return false;

	std::set<fs::path> Visited;
	return AreShaderSourcesOlder(ShaderPath, CsoTime, Visited);
}

FShaderByteCode RenderUtil::GetOrCompile(const FString& ShaderPath, const char* EntryPoint, EShaderType ShaderType, FString& OutCSOPath, const D3D_SHADER_MACRO* Defines, const FString& Variant)
{
	OutCSOPath = GetCsoPath(ShaderPath, ShaderType, Variant);

	if (IsCsoUpToDate(ShaderPath, OutCSOPath))
	{
		FShaderByteCode Cached = LoadByteCode(OutCSOPath);
		if (Cached.IsValid()) return Cached;
	}

	FShaderByteCode Compiled = CompileShader(ShaderPath, EntryPoint, ShaderType, Defines);
	if (Compiled.IsValid())
		SaveByteCode(OutCSOPath, Compiled);

	return Compiled;

}

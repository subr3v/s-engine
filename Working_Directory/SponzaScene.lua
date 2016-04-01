-- Scene Configuration

function Vector(X, Y, Z)
	return { x = X, y = Y, z = Z }
end

function range(from, to, step)
  step = step or 1
  return function(_, lastvalue)
    local nextvalue = lastvalue + step
    if step > 0 and nextvalue <= to or step < 0 and nextvalue >= to or
       step == 0
    then
      return nextvalue
    end
  end, nil, from - step
end


SceneName = "Sponza"

ResourcePacks =
{
	"Sponza.pak",
	"SponzaModels.pak",
	"Conference.pak",
	"Sibenik.pak"
}

Models = 
{
	{
		modelPath = "sponza.obj",
		position = Vector(0, 0, 0),
		scale = Vector(1, 1, 1),
		rotation = Vector(0, 0, 0)
	}
}

PointLights = 
{

}

for i in range(0, 3000) do
	xPos = - 1600 + math.random() * 3200
	yPos = - 512 + math.random() * 1600
	zPos = - 1600 + math.random() * 3200
	PointLights[#PointLights + 1] = 
	{
		colour = Vector(math.random(), math.random(), math.random()),
		position = Vector(xPos, yPos, zPos),
		intensity = 0.5 + math.random() * 100,
		radius = 128,
	}
end

SpotLights = 
{
	{
		colour = Vector(1, 1, 1),
		direction = Vector(1, -0.5, 0), 
		position = Vector(125, 350, 00),
		phi = 45,
		theta = 45,
		intensity = 25,
		radius = 2500,
		castShadow = true
	},
		{
		colour = Vector(1, 1, 1),
		direction = Vector(-1, -0.5, 0), 
		position = Vector(125, 350, 00),
		phi = 45,
		theta = 60,
		intensity = 12,
		radius = 3200,
		castShadow = true
	},
	--[[ This one is nice for conference room, keep it real.
	{
		colour = Vector(1, 1, 1),
		direction = Vector(1, -0.5, 0), 
		position = Vector(-300, 350, 00),
		phi = 45,
		theta = 45,
		intensity = 25,
		radius = 2500,
		castShadow = false
	},
	]]--
	{
		colour = Vector(1, 1, 1),
		direction = Vector(1, 0.0, 1.0), 
		position = Vector(135, 250, 25),
		phi = 15,
		theta = 60,
		intensity = 50,
		radius = 1200,
		castShadow = true
	},
	{
		colour = Vector(1, 1, 1),
		direction = Vector(1, 0.0, 1.0), 
		position = Vector(105, 650, -155),
		phi = 15,
		theta = 60,
		intensity = 0,
		radius = 1200,
		castShadow = true
	}
}

SpotLights = { }

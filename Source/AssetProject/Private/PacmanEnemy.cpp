// Fill out your copyright notice in the Description page of Project Settings.


#include "PacmanEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Pacman.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APacmanEnemy::APacmanEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsDead = false;
	bIsVulnerable = false;

	DeadTime = 5.0f;
	VulnerableTime = 5.0f;
	
	NormalSpeed = 300.0f;
	VulnerableSpeed = 100.0f;
	
	Enemy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy"));
	Enemy->SetupAttachment(GetCapsuleComponent());
}

// Called when the game starts or when spawned
void APacmanEnemy::BeginPlay()
{
	Super::BeginPlay();
	SetMovement(true);
	StartLocation = GetActorLocation();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this,&APacmanEnemy::OnEnemyBeginOverlay);

}

// Called every frame
void APacmanEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APacmanEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APacmanEnemy::SetVulnerable(bool bRestoreTimer, float Time)
{
	if(bRestoreTimer)
	{
		GetWorldTimerManager().SetTimer(VulnerableTimerHandle,this,&APacmanEnemy::SetNormal,Time,false);
	}else
	{
		GetWorldTimerManager().SetTimer(VulnerableTimerHandle,this,&APacmanEnemy::SetNormal,VulnerableTime,false);
	}
	if(bIsVulnerable)
	{
		return;
	}
	bIsVulnerable = true;
	SetMovement(true);
}

void APacmanEnemy::SetDead(bool bRestoreTimer, float Time)
{
	if(bRestoreTimer && bIsDead)
	{
		GetWorldTimerManager().SetTimer(DeadTimerHandle,this,&APacmanEnemy::SetNormal,Time,false);
		return;
	}
	if(bIsDead)
	{
		return;
	}
	bIsDead = true;
	SetMovement(true);
	SetActorLocation(StartLocation);
	GetWorldTimerManager().ClearTimer(VulnerableTimerHandle);
	GetWorldTimerManager().SetTimer(DeadTimerHandle,this,&APacmanEnemy::SetNormal,DeadTime,false);
}

void APacmanEnemy::SetNormal()
{
	GetWorldTimerManager().ClearTimer(DeadTimerHandle);
	GetWorldTimerManager().ClearTimer(VulnerableTimerHandle);
	bIsDead = false;
	bIsVulnerable = false;
	SetMovement(true);
}

void APacmanEnemy::SetMovement(bool bCanMove)
{
	if(bCanMove)
	{
		if(!bIsVulnerable && !bIsDead)
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}else if(bIsVulnerable && !bIsDead)
		{
			GetCharacterMovement()->MaxWalkSpeed = VulnerableSpeed;
		}else if(bIsDead)
		{
			GetCharacterMovement()->MaxWalkSpeed = 0;
		}
	}else
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}
}

void APacmanEnemy::OnEnemyBeginOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APacman::StaticClass()))
	{
		if(bIsDead)
		{
			return;
		}
		if(bIsVulnerable)
		{
			SetDead();
		}else
		{
			Cast<APacman>(OtherActor)->InJured();
		}
	}
}


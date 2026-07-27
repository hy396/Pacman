// Fill out your copyright notice in the Description page of Project Settings.


#include "Pacman.h"

#include "MyGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "pacdot.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Public/TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APacman::APacman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Lifes = 3;
	NormalSpeed = 600;
	Velocity = FVector(0,0,0);

	wuditime = 3.0f;
	
	Pacman = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pacman"));
	Pacman->SetupAttachment(GetCapsuleComponent());

}

// Called when the game starts or when spawned
void APacman::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	GameModeRef = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this));
	SetMovement(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this,&APacman::OnPacmanBeginOverlay);
}

// Called every frame
void APacman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APacman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("StartGame",IE_Pressed,this,&APacman::StartGame);
	PlayerInputComponent->BindAction("PauseGame",IE_Pressed,this,&APacman::PauseGame);
	PlayerInputComponent->BindAction("RestartGame",IE_Pressed,this,&APacman::RestartGame);
	
	PlayerInputComponent->BindAxis("MoveX",this,&APacman::MoveX);
	PlayerInputComponent->BindAxis("MoveY",this,&APacman::MoveY);
}

void APacman::InJured()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("wudi"));
	Lifes--;
	if(Lifes == 0)
	{
		GameModeRef->SetCurrentState(EGameState::EGameOver);
	}else
	{
		SetActorLocation(StartLocation);
		GetWorldTimerManager().SetTimer(ResettingTimerHandle,this,&APacman::Resetting,wuditime,false);
	}
}

void APacman::Resetting()
{
	GetWorldTimerManager().ClearTimer(ResettingTimerHandle);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}

void APacman::SetMovement(bool bCanMove)
{
	if(bCanMove)
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}else
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;		
	}
}

void APacman::StartGame()
{
	GameModeRef->StartGame();
}

void APacman::PauseGame()
{
	GameModeRef->PauseGame();
}

void APacman::RestartGame()
{
	GameModeRef->RestartGame();
}

void APacman::MoveX(float Value)
{
	if(GameModeRef->GetCurrentState() == EGameState::EPlay)
	{
		if(Controller && Value != 0)
		{
			Velocity.X = Value;
			Velocity.Y = 0;
			if(Value>0)
			{
				SetActorRotation(FRotator(0,0,0));
			}else
			{
				SetActorRotation(FRotator(0,-180,0));
			}
			AddMovementInput(Velocity);
		}
	}
}

void APacman::MoveY(float Value)
{
	if(GameModeRef->GetCurrentState() == EGameState::EPlay)
	{
		if(Controller && Value != 0)
		{
			Velocity.Y = Value;
			Velocity.X = 0;
			if(Value>0)
			{
				SetActorRotation(FRotator(0,90,0));
			}else
			{
				SetActorRotation(FRotator(0,-90,0));
			}
			AddMovementInput(Velocity);
		}
	}
}

void APacman::OnPacmanBeginOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult & SweepResult)
{
	if(GameModeRef->GetCurrentState() == EGameState::EPlay)
	{
		if(OtherActor->IsA(Apacdot::StaticClass()))
		{
			Apacdot* CurrentPacdot = Cast<Apacdot>(OtherActor);
			if(CurrentPacdot->bIsSuperPacdot)
			{
				GameModeRef->SetEnemiesVulnerable();
			}
			OtherActor->Destroy();
			GameModeRef->SetPacdotNum(GameModeRef->GetPacdotNum() - 1);
		}
	}
}


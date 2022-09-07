// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	bUseControllerRotationYaw = false; // Dont rotate the character with camera
	GetCharacterMovement()->bOrientRotationToMovement = true; // Move Character in the direction of rotation
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PHandle = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UPhysicsHandleComponent>();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PHandle != nullptr)
	{
		FVector Location;
		FRotator Rotation;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Location, Rotation);

		if (PHandle->GrabbedComponent)
		{
			PHandle->SetTargetLocation(Location + Rotation.Vector() * 500.f);
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AMainCharacter::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AMainCharacter::Right);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Grab"), IE_Pressed, this, &AMainCharacter::Grab);
	PlayerInputComponent->BindAction(TEXT("Release"), IE_Pressed, this, &AMainCharacter::Release);

}

void AMainCharacter::Forward(float AxisValue)
{
	const FRotator Rotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

	AddMovementInput(Direction, AxisValue);
}

void AMainCharacter::Right(float AxisValue)
{
	const FRotator Rotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Direction, AxisValue);
}

void AMainCharacter::Grab()
{
	if (PHandle != nullptr)
	{
		FVector Location;
		FRotator Rotation;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Location, Rotation);

		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
		GetWorld()->LineTraceSingleByObjectType(Hit, Location, Location + Rotation.Vector() * 500.f, ECC_PhysicsBody, TraceParams);

		if (Hit.GetActor())
		{
			UParticleSystemComponent* Particle = Hit.GetActor()->FindComponentByClass<UParticleSystemComponent>();
			if (Particle)
			{
				Particle->Deactivate();
			}

			PHandle->GrabComponent(Hit.GetComponent(), NAME_None, Location + Rotation.Vector() * 500.f, true);
		}
	}
	
}

void AMainCharacter::Release()
{
	if (PHandle != nullptr)
	{
		PHandle->ReleaseComponent();
	}
}
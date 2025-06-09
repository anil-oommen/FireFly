import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { VirtuJoystickComponent } from './virtu-joystick.component';

describe('VirtuJoystickComponent', () => {
  let component: VirtuJoystickComponent;
  let fixture: ComponentFixture<VirtuJoystickComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ VirtuJoystickComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(VirtuJoystickComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});

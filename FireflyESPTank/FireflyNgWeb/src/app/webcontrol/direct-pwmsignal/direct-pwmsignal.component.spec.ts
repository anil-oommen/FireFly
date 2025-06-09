import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DirectPwmsignalComponent } from './direct-pwmsignal.component';

describe('DirectPwmsignalComponent', () => {
  let component: DirectPwmsignalComponent;
  let fixture: ComponentFixture<DirectPwmsignalComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DirectPwmsignalComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DirectPwmsignalComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
